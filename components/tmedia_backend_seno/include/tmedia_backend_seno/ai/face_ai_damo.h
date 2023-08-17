/*
 * Copyright (c) 2022-2022 Alibaba Group Holding Limited
 */

#ifndef __FACE_AI_DAMO_H__
#define __FACE_AI_DAMO_H__

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/ai/face_ai.h>

#define CX_FACEAI_INPUT_WIDTH 512
#define CX_FACEAI_INPUT_HEIGHT 640
#define CX_FACEAI_INPUT_CHN    1

struct FaceNet_Nano_Result_s;
typedef struct FaceNet_Nano_Result_s FaceNet_Nano_Result;
struct FaceNet_ImageFrame_s;
typedef struct FaceNet_ImageFrame_s FaceNet_ImageFrame;

typedef enum {
    FACE_DETECT_THRES = 0,
    KEYPOINT_THRES,
    LIVING_2D_THRES,
    LIVING_3D_THRES,
    FACE_QUALITY_THRES,
    FACE_RECO_THRES,
} face_ai_config_e;

class FaceAIDamo : public FaceAIEntity {
public:
    FaceAIDamo();
    ~FaceAIDamo();

    // Face AI interface 
    void RegisterModelLoader(face_ai_load_model_callback_t model_loader);
    int LoadModels(std::vector<std::string> &model_paths, std::vector<ModelParam*> &model_params);  // optional
    int InitAlgo(std::vector<ModelParam*> &model_params);
    int SetConfig(TMPropertyList &propList);
    int GetConfig(TMPropertyList &propList);
    int Destroy();

    int FaceDetect(std::vector<TMVideoFrame*>* frame_list, std::vector<DetectBox*> &box_list);
    int QualityFilter(std::vector<TMVideoFrame*>* frame_list);  //quality & landmark
    int AntiSpoofing(std::vector<TMVideoFrame*>* frame_list);  //monocular & binocular
    int FeatureExtract(std::vector<TMVideoFrame*> *frame_list, float **feature, int *feature_length);
    int CompareFeature(float* feature, float** feature_list, uint16_t* id_list, int size, float* score);

    // TMFilterEntity interface
    TMSrcPad *GetSrcPad(int padID = 0);
    TMSinkPad *GetSinkPad(int padID = 0);

private:
    FaceNet_Nano_Result *face_result_;
    std::vector<ModelParam*> model_params_;
    TMPropertyList mDefaultPropertyList;
    TMPropertyList mCurrentPropertyList;
    uint32_t init_flags_ = 0;
    face_ai_load_model_callback_t model_loader_ = nullptr;

#if (CONFIG_FACEAI_BINOCULAR == 2)
    int detection_index_ = 1;
#endif

    void SetDefaultFrame(FaceNet_ImageFrame* frame, uint8_t* data);
    void InitDefaultPropertyList();
    int InitFaceNetNano(uint32_t model_id, const std::string& model_name, const std::string& file_name);
};

#endif // __FACE_AI_DAMO_H__