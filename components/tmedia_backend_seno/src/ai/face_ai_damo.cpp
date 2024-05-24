/*
 * Copyright (c) 2022-2022 Alibaba Group Holding Limited
 */

#include <tmedia_backend_seno/ai/face_ai_damo.h>

#include <sailface_sdk/include/face_net_types.h>
#include <sailface_sdk/include/nano_face_pipeline.h>

#ifdef CONFIG_TMEDIA_ALGO_LOG
#define AILOGI(mod, format, ...) printf("["#mod"] "#format"\n", ##__VA_ARGS__)
#define AILOGD(mod, format, ...) printf("["#mod"] "#format"\n", ##__VA_ARGS__)
#define AILOGW(mod, format, ...) printf("["#mod"] "#format"\n", ##__VA_ARGS__)
#define AILOGE(mod, format, ...) printf("["#mod"] "#format"\n", ##__VA_ARGS__)
#else
#define AILOGI(mod, format, ...)
#define AILOGD(mod, format, ...)
#define AILOGW(mod, format, ...) printf("["#mod"] "#format"\n", ##__VA_ARGS__)
#define AILOGE(mod, format, ...) printf("["#mod"] "#format"\n", ##__VA_ARGS__)
#endif

#define FACEAI_PROP_GET_INT(my_prop_list, in_prop_list, type) do {\
        int value;\
        if(propList.Get(type, &value) == 0) {\
            if(value < 0){return TMResult::TM_EINVAL;}\
            my_prop_list.Assign(type, value);}\
        } while(0)

FaceAIDamo::FaceAIDamo()
{
    InitDefaultPropertyList();
}

FaceAIDamo::~FaceAIDamo()
{
    
}

void FaceAIDamo::RegisterModelLoader(face_ai_load_model_callback_t model_loader)
{
    model_loader_ = model_loader;
    return;
}

int FaceAIDamo::LoadModels(std::vector<std::string> &model_paths, std::vector<ModelParam*> &model_params)
{
    return TMResult::TM_OK;
}

int FaceAIDamo::InitAlgo(std::vector<ModelParam*> &model_params)
{
    // create face net
    FaceNet_ImageFrame frames[2];
    SetDefaultFrame(&frames[0], NULL);
    SetDefaultFrame(&frames[1], NULL);

    FaceNetError ret = FaceNet_Nano_Create(frames, sizeof(frames) / sizeof(FaceNet_ImageFrame), NULL);
    if (ret != FACENET_ERR_OK) {
        AILOGE(FaceAIDamo, "FaceNet_Nano_Create failed, result code is %d.", ret);
        return TMResult::TM_STATE_ERROR;
    }
    
    FaceNet_Threshold threshold{
        .facedetect_thres = ((float)mCurrentPropertyList.GetInt(face_ai_config_e::FACE_DETECT_THRES)) / 1000,
        .ptscore_thres = ((float)mCurrentPropertyList.GetInt(face_ai_config_e::KEYPOINT_THRES)) / 1000,
        .living2d_thres = ((float)mCurrentPropertyList.GetInt(face_ai_config_e::LIVING_2D_THRES)) / 1000,
        .living3d_thres = ((float)mCurrentPropertyList.GetInt(face_ai_config_e::LIVING_3D_THRES)) / 1000,
        .facequality_thres = ((float)mCurrentPropertyList.GetInt(face_ai_config_e::FACE_QUALITY_THRES)) / 1000,
        .face_reco_thres = ((float)mCurrentPropertyList.GetInt(face_ai_config_e::FACE_RECO_THRES)) / 1000
    };
    AILOGD(FaceAIDamo, "FaceAI, threshold: %.3f, %.3f, %.3f, %.3f, %.3f, %.3f", threshold.facedetect_thres, threshold.ptscore_thres, \
    threshold.living2d_thres, threshold.living3d_thres, threshold.facequality_thres, threshold.face_reco_thres);
    FaceNet_Nano_Set_Params(&threshold);

    // init face result
    face_result_ = new FaceNet_Nano_Result();
    memset(face_result_, 0, sizeof(FaceNet_Nano_Result));

    return TMResult::TM_OK;
}

int FaceAIDamo::SetConfig(TMPropertyList &propList)
{
    vector<int> diffIDArray;
    int diffCount = mCurrentPropertyList.FindDiff(diffIDArray, &propList);

    if (diffCount == 0)
    {
        AILOGI(FaceAIDamo, "No different config found");
        return TMResult::TM_OK;
    }

    FACEAI_PROP_GET_INT(mCurrentPropertyList, propList, face_ai_config_e::FACE_DETECT_THRES);
    FACEAI_PROP_GET_INT(mCurrentPropertyList, propList, face_ai_config_e::KEYPOINT_THRES);
    FACEAI_PROP_GET_INT(mCurrentPropertyList, propList, face_ai_config_e::LIVING_2D_THRES);
    FACEAI_PROP_GET_INT(mCurrentPropertyList, propList, face_ai_config_e::LIVING_3D_THRES);
    FACEAI_PROP_GET_INT(mCurrentPropertyList, propList, face_ai_config_e::FACE_QUALITY_THRES);
    FACEAI_PROP_GET_INT(mCurrentPropertyList, propList, face_ai_config_e::FACE_RECO_THRES);

    return TMResult::TM_OK;
}

int FaceAIDamo::GetConfig(TMPropertyList &propList)
{
    propList.Reset();
    TMPropertyMap_t *currProps = &(mCurrentPropertyList.mProperties);
    for (auto iter = currProps->begin(); iter != currProps->end(); iter++)
    {
        TMProperty *prop = &iter->second;
        propList.Add(prop);
    }
    return TMResult::TM_OK;
}

int FaceAIDamo::Destroy()
{
    FaceNet_Nano_Uninit(FACE_MODEL_ID_DETECTION);
    FaceNet_Nano_Uninit(FACE_MODEL_ID_LANDMARK);
    FaceNet_Nano_Uninit(FACE_MODEL_ID_QUALITY);
    FaceNet_Nano_Uninit(FACE_MODEL_ID_RGB_IR_LIVENESS);
    FaceNet_Nano_Uninit(FACE_MODEL_ID_DEPTH_IR_LIVENESS);
    FaceNet_Nano_Uninit(FACE_MODEL_ID_RECO);
    init_flags_ = 0;
    model_loader_ = nullptr;

    FaceNet_Nano_Destroy();
    return TMResult::TM_OK;
}

int FaceAIDamo::FaceDetect(std::vector<TMVideoFrame*>* frame_list, std::vector<DetectBox*> &box_list)
{
    assert(frame_list->size() == 2);
    assert((*frame_list)[0]->mPixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_GRAY);
    assert((*frame_list)[0]->mHeight == CX_FACEAI_INPUT_HEIGHT);
    assert((*frame_list)[0]->mWidth == CX_FACEAI_INPUT_WIDTH);
    assert((*frame_list)[0]->mPlanes == 1);

    FaceNet_ImageFrame frames[2];
    for (int i = 0; (size_t)i < frame_list->size(); ++i) {
        SetDefaultFrame(&frames[i], (*frame_list)[i]->mData[0]);
    }

    /**
        monocular: right image
        binocular: left image
        bin-monocular: 1st right image, 2nd left image
    **/
#if (CONFIG_FACEAI_BINOCULAR != 2)
    FaceNetError ret = FaceNet_Nano_Set_Input(frames, 0);
#else
    FaceNetError ret = FaceNet_Nano_Set_Input(frames, detection_index_);
    if (detection_index_ == 0) {
        detection_index_ = 1;
    }
#endif

    if (ret != FACENET_ERR_OK) {
        AILOGE(FaceAIDamo, "FaceNet_Nano_Set_Input failed, result code is %d.", ret);
        return TMResult::TM_STATE_ERROR;
    }

    if (!(init_flags_ & (1 << 0))) {
        if (InitFaceNetNano(FACE_MODEL_ID_DETECTION, "model_facedet", "") != 0) {
            AILOGE(FaceAIDamo, "InitFaceNetNano failed.");
            return TMResult::TM_STATE_ERROR;
        }

        init_flags_ = init_flags_ | (1 << 0);
    }

    ret = FaceNet_Nano_Forward(FACE_MODEL_ID_DETECTION, face_result_);
    AILOGD(FaceAIDamo, "face num:%d det box:[%d, %d], [%d, %d] score:%.3f", face_result_->face_count, 
    face_result_->face_box.x1, face_result_->face_box.y1, face_result_->face_box.x2, face_result_->face_box.y2,
    face_result_->face_box.score);
    if (ret != FACENET_ERR_OK) {
        AILOGE(FaceAIDamo, "FaceNet_Nano_Forward FACE_MODEL_ID_DETECTION failed, result code is %d, (score: %.3f).", ret, face_result_->face_box.score);
        return TMResult::TM_STATE_ERROR;
    }

    return TMResult::TM_OK;
}

int FaceAIDamo::QualityFilter(std::vector<TMVideoFrame*>* frame_list)
{
    // landmark filter
    FaceNetError ret = FACENET_ERR_UNKNOWN_ERR;
    if (!(init_flags_ & (1 << 1))) {
        if (InitFaceNetNano(FACE_MODEL_ID_LANDMARK, "model_faceldmk", "") != 0) {
            AILOGE(FaceAIDamo, "InitFaceNetNano failed.");
            return TMResult::TM_STATE_ERROR;
        }
        init_flags_ = init_flags_ | (1 << 1);
    }

    ret = FaceNet_Nano_Forward(FACE_MODEL_ID_LANDMARK, face_result_);

    AILOGD(FaceAIDamo, "landmark:((%d, %d), (%d, %d), (%d, %d), (%d, %d), (%d, %d))", (int)face_result_->lamdmarkPoints[0], 
    (int)face_result_->lamdmarkPoints[1], (int)face_result_->lamdmarkPoints[2], (int)face_result_->lamdmarkPoints[3], 
    (int)face_result_->lamdmarkPoints[4], (int)face_result_->lamdmarkPoints[5], (int)face_result_->lamdmarkPoints[6],
    (int)face_result_->lamdmarkPoints[7], (int)face_result_->lamdmarkPoints[8], (int)face_result_->lamdmarkPoints[9]);

    if (ret != FACENET_ERR_OK) {
        AILOGE(FaceAIDamo, "FaceNet_Nano_Forward FACE_MODEL_ID_LANDMARK failed, result code is %d, score: %.3f, threshold is: %.3f.", ret, face_result_->landmarkScore, ((float)mCurrentPropertyList.GetInt(face_ai_config_e::KEYPOINT_THRES)) / 1000);
        return TMResult::TM_STATE_ERROR;
    }
    AILOGD(FaceAIDamo, "FACE_MODEL_ID_LANDMARK , landmarkScore is %.3f.", face_result_->landmarkScore);

    // image quality filter
    if (!(init_flags_ & (1 << 2))) {
        if (InitFaceNetNano(FACE_MODEL_ID_QUALITY, "model_quality", "") != 0) {
            AILOGE(FaceAIDamo, "InitFaceNetNano failed.");
            return TMResult::TM_STATE_ERROR;
        }
        init_flags_ = init_flags_ | (1 << 2);
    }

    ret = FaceNet_Nano_Forward(FACE_MODEL_ID_QUALITY, face_result_);
    
    if (ret != FACENET_ERR_OK) {
        AILOGE(FaceAIDamo, "FaceNet_Nano_Forward FACE_MODEL_ID_QUALITY failed, result code is %d, score: %.3f, threshold is: %.3f.", ret, ((float)face_result_->qualityScore / 100), ((float)mCurrentPropertyList.GetInt(face_ai_config_e::FACE_QUALITY_THRES)) / 1000);
        return TMResult::TM_STATE_ERROR;
    }
    AILOGD(FaceAIDamo, "FACE_MODEL_ID_QUALITY , qualityScore is %.3f.", ((float)face_result_->qualityScore / 100));

    return TMResult::TM_OK;
}

int FaceAIDamo::AntiSpoofing(std::vector<TMVideoFrame*>* frame_list)
{
    // monocular ir liveness
    FaceNetError ret = FACENET_ERR_UNKNOWN_ERR;
    if (!(init_flags_ & (1 << 3))) {
        if (InitFaceNetNano(FACE_MODEL_ID_RGB_IR_LIVENESS, "model_single_ir", "") != 0) {
            AILOGE(FaceAIDamo, "InitFaceNetNano failed.");
            return TMResult::TM_STATE_ERROR;
        }
        init_flags_ = init_flags_ | (1 << 3);
    }

    ret = FaceNet_Nano_Forward(FACE_MODEL_ID_RGB_IR_LIVENESS, face_result_);
    
    if (ret != FACENET_ERR_OK) {
        AILOGE(FaceAIDamo, "FaceNet_Nano_Forward FACE_MODEL_ID_IR_LIVENESS failed, result code is %d, score: %.3f, threshold is: %.3f.", ret, ((float)face_result_->antilcScore / 100), ((float)mCurrentPropertyList.GetInt(face_ai_config_e::LIVING_2D_THRES)) / 1000);
        return TMResult::TM_STATE_ERROR;
    }
    AILOGD(FaceAIDamo, "FACE_MODEL_ID_IR_LIVENESS , antilcScore is %.3f.", ((float)face_result_->antilcScore / 100));

#if (CONFIG_FACEAI_BINOCULAR == 1)
    // binocular ir liveness
    if (!(init_flags_ & (1 << 4))) {
        if (InitFaceNetNano(FACE_MODEL_ID_DEPTH_IR_LIVENESS, "model_ht", "rectify_calib_remap") != 0) {
            AILOGE(FaceAIDamo, "InitFaceNetNano failed.");
            return TMResult::TM_STATE_ERROR;
        }
        init_flags_ = init_flags_ | (1 << 4);
    }

    assert(frame_list->size() == 2);
    assert((*frame_list)[0]->mPixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_GRAY);
    assert((*frame_list)[0]->mHeight == CX_FACEAI_INPUT_HEIGHT);
    assert((*frame_list)[0]->mWidth == CX_FACEAI_INPUT_WIDTH);
    assert((*frame_list)[0]->mPlanes == 1);

    FaceNet_ImageFrame frames[2];
    for (int i = 0; i < frame_list->size(); ++i) {
        SetDefaultFrame(&frames[i], (*frame_list)[i]->mData[0]);
    }

    ret = FaceNet_Nano_Set_StereoDepth_Input(frames, sizeof(frames) / sizeof(FaceNet_ImageFrame));
    ret = FaceNet_Nano_Forward(FACE_MODEL_ID_DEPTH_IR_LIVENESS, face_result_);

    if (ret != FACENET_ERR_OK) {
        AILOGE(FaceAIDamo, "FaceNet_Nano_Forward FACE_MODEL_ID_DEPTH_IR_LIVENESS failed, result code is %d, score: %.3f, threshold is: %.3f.", ret, ((float)face_result_->antidpScore / 100), ((float)mCurrentPropertyList.GetInt(face_ai_config_e::LIVING_3D_THRES)) / 1000);
        return TMResult::TM_STATE_ERROR;
    }
    AILOGD(FaceAIDamo, "FACE_MODEL_ID_DEPTH_IR_LIVENESS , antidpScore is %.3f.", ((float)face_result_->antidpScore / 100));
#elif (CONFIG_FACEAI_BINOCULAR == 2)
    if (detection_index_ == 1) {
        detection_index_ = 0;
    }
#endif

    return TMResult::TM_OK;
}

int FaceAIDamo::FeatureExtract(std::vector<TMVideoFrame*> *frame_list, float **feature, int *feature_length)
{
    FaceNetError ret = FACENET_ERR_UNKNOWN_ERR;
    if (!(init_flags_ & (1 << 5))) {
        if (InitFaceNetNano(FACE_MODEL_ID_RECO, "model_facerecog", "") != 0) {
            AILOGE(FaceAIDamo, "InitFaceNetNano failed.");
            return TMResult::TM_STATE_ERROR;
        }
        init_flags_ = init_flags_ | (1 << 5);
    }

    ret = FaceNet_Nano_Forward(FACE_MODEL_ID_RECO, face_result_);
    
    if (ret != FACENET_ERR_OK) {
        AILOGE(FaceAIDamo, "FaceNet_Nano_Forward FACE_MODEL_ID_RECO failed, result code is %d.", ret);
        return TMResult::TM_STATE_ERROR;
    }
    AILOGD(FaceAIDamo, "feature info: c:%d, h:%d, w:%d, size:%d", face_result_->feature.c, face_result_->feature.h, face_result_->feature.w, face_result_->feature.elemSize);
    *feature = (float*)face_result_->feature.data;

    return TMResult::TM_OK;
}

int FaceAIDamo::CompareFeature(float* feature, float** feature_list, uint16_t* id_list, int size, float* score)
{
    FaceNet_Data face;
    face.c = 512;
    face.h = 1;
    face.w = 1;
    face.elemSize = 4;
    face.data = feature;

    int index = FaceNet_Nano_Compare_Feature(&face, feature_list, id_list, size, score);

    if (index == -1) {
        AILOGE(FaceAIDamo, "Feature compare failed, index: %d, max similarity score: %f, threshold: %.3f", index, *score, ((float)mCurrentPropertyList.GetInt(face_ai_config_e::FACE_RECO_THRES)) / 1000);
    }

    AILOGD(FaceAIDamo, "Feature compare, index: %d, score: %f", index, *score);

    return index;
}

TMSrcPad* FaceAIDamo::GetSrcPad(int padID)
{
    return nullptr;
}

TMSinkPad* FaceAIDamo::GetSinkPad(int padID)
{
    return nullptr;
}

void FaceAIDamo::SetDefaultFrame(FaceNet_ImageFrame* frame, uint8_t* data)
{
    frame->c = CX_FACEAI_INPUT_CHN;
    frame->w = CX_FACEAI_INPUT_WIDTH;
    frame->h = CX_FACEAI_INPUT_HEIGHT;
    frame->fmt = FACENET_PIXFMT_GRAY;
    frame->data = data;
    return;
}

void FaceAIDamo::InitDefaultPropertyList()
{
    TMPropertyList *pList[2] = {&mDefaultPropertyList, &mCurrentPropertyList};
    for (uint32_t i = 0; i < ARRAY_SIZE(pList); i++)
    {
        pList[i]->Add(TMProperty((int)face_ai_config_e::FACE_DETECT_THRES, 450, "face detection threshold"));
        pList[i]->Add(TMProperty((int)face_ai_config_e::KEYPOINT_THRES, 145, "face landmark threshold"));
        pList[i]->Add(TMProperty((int)face_ai_config_e::LIVING_2D_THRES, 830, "living 2d threshold"));
        pList[i]->Add(TMProperty((int)face_ai_config_e::LIVING_3D_THRES, 900, "living 3d threshold"));
        pList[i]->Add(TMProperty((int)face_ai_config_e::FACE_QUALITY_THRES, 400, "face quality threshold"));
        pList[i]->Add(TMProperty((int)face_ai_config_e::FACE_RECO_THRES, 710, "face recognition threshold"));
    }
}

int FaceAIDamo::InitFaceNetNano(uint32_t model_id, const std::string& model_name, const std::string& file_name)
{
    FaceNet_Model_ID_t model_id_inner = static_cast<FaceNet_Model_ID_t>(model_id);
    FaceNet_Model_Params_t model_param;
    memset(&model_param, 0, sizeof(FaceNet_Model_Params_t));
    model_param.numbers = 1;
    model_param.model_params[0].id = model_id_inner;
    if (model_loader_ == nullptr) {
        AILOGE(FaceAIDamo, "Model loader is null, please RegisterModelLoader firstly");
        return TMResult::TM_STATE_ERROR;
    }

    ModelAddress model_addr;
    int load_ret = model_loader_(model_name, model_addr);
    if (load_ret != 0) {
        AILOGE(FaceAIDamo, "model load failed, model id is: %d, model name is %s, result code is %d.", model_id_inner, model_name.c_str(), load_ret);
        return TMResult::TM_STATE_ERROR;
    }
    model_param.model_params[0].weight = (void *)model_addr.ramAddr;
    model_param.model_params[0].weight_size = model_addr.size;

    if (file_name.size() > 0) {
        ModelAddress file_addr;
        int load_ret = model_loader_(file_name, file_addr);
        if (load_ret != 0) {
            AILOGE(FaceAIDamo, "model load failed, model id is: %d, file name is %s, result code is %d.", model_id_inner, file_name.c_str(), load_ret);
            return TMResult::TM_STATE_ERROR;
        }
        model_param.model_params[0].reserved = (void *)file_addr.ramAddr;
        model_param.model_params[0].reserved_size = file_addr.size;
    }

    FaceNetError ret = FaceNet_Nano_Init(&model_param);
    if (ret != FACENET_ERR_OK) {
        AILOGE(FaceAIDamo, "FaceNet_Nano_Init failed, model id is: %d, model name is %s, result code is %d.", model_id_inner, model_name.c_str(), ret);
        FaceNet_Nano_Uninit(model_id_inner);
        return TMResult::TM_STATE_ERROR;
    }

    return TMResult::TM_OK;
}