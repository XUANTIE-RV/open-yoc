/*
 * Copyright (c) 2022-2022 Alibaba Group Holding Limited
 */
#ifndef __FACE_AI_H__
#define __FACE_AI_H__

#include <tmedia_core/entity/entity.h>

typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
    float score;
} DetectBox;

typedef struct {
    void* param;
    uint32_t length;
} ModelParam;

typedef struct {
    uint64_t flashAddr;
    uint64_t ramAddr;
    uint32_t size;
} ModelAddress;

typedef int (*face_ai_load_model_callback_t)(const std::string &model_name, ModelAddress &model_addr);

class FaceAIEntity : public TMFilterEntity
{
public:
        FaceAIEntity(){};
        virtual ~FaceAIEntity(){};

        // Face AI interface 
        /**
         * @description: Register fast boot model loader.
         * @param [in] model_loader: model loader callback.
         * @return [void]
         */    
        virtual void RegisterModelLoader(face_ai_load_model_callback_t model_loader) = 0;

        /**
         * @description: Load models from model paths.
         * @param [in] &model_paths: model paths
         * @param [inout] &model_params: Parameter pointer and length of models.
         * @return [int] 0:success, -1:failure
         */    
        virtual int LoadModels(std::vector<std::string> &model_paths, std::vector<ModelParam*> &model_params) = 0;
        /**
         * @description: Initialize algorithm
         * @param [in] &model_params: Parameter pointer and length of models, get from LoadModels or other means.
         * @return [int] 0:success, -1:failure
         */    
        virtual int InitAlgo(std::vector<ModelParam*> &model_params) = 0;
        /**
         * @description: Set configuration.
         * @param [in] &propList: Property list of algorithm.
         * @return [int] 0:success, -1:failure
         */        
        virtual int SetConfig(TMPropertyList &propList) = 0;
        /**
         * @description: Get configuration.
         * @param [out] &propList: Property list of algorithm.
         * @return [int] 0:success, -1:failure
         */        
        virtual int GetConfig(TMPropertyList &propList) = 0;
        /**
         * @description: Destroy algorithm.
         * @return [int] 0:success, -1:failure
         */        
        virtual int Destroy() = 0;
        /**
         * @description: Face detection
         * @param [in] *frame_list: Input frames.
         * @param [out] &box_list: Bounding box list of faces.
         * @return [int] 0:success, -1:failure
         */        
        virtual int FaceDetect(std::vector<TMVideoFrame*> *frame_list, std::vector<DetectBox*> &box_list) = 0;
        /**
         * @description: Check whether the face image meets the quality requirements. The requirements are defined by the algorithm provider.
         * @param [in] *frame_list: One image or one set of images.
         * @return [int] 0:success, -1:failure
         */        
        virtual int QualityFilter(std::vector<TMVideoFrame*> *frame_list) = 0;
        /**
         * @description: Check whether the face is fake.
         * @param [in] *frame_list: One image or one set of images.
         * @return [int] 0:success, -1:failure
         */        
        virtual int AntiSpoofing(std::vector<TMVideoFrame*> *frame_list) = 0;
        /**
         * @description: Extract the feature of one face.
         * @param [in] *frame_list: One image or one set of images.
         * @param [float] **feature: One face feature.
         * @param [int] *feature_length: The length of face feature.
         * @return [int] 0:success, -1:failure
         */        
        virtual int FeatureExtract(std::vector<TMVideoFrame*> *frame_list, float **feature, int *feature_length) = 0;
        /**
         * @description: Compare the similarity between the face feature and the registered features.
         * @param [in] *feature: The feature to be compared.
         * @param [in] **feature_list: Registered feature array.
         * @param [in] *id_list: Registered feature ids.
         * @param [in] size: list size.
         * @param [out] *score: similarity score
         * @return [int] index of max similarity, return -1 when no feature passes the threshold.
         */        
        virtual int CompareFeature(float *feature, float **feature_list, uint16_t *id_list, int size, float *score) = 0;

        // TMFilterEntity interface
        virtual TMSrcPad *GetSrcPad(int padID = 0) = 0;
        virtual TMSinkPad *GetSinkPad(int padID = 0) = 0;

};
#endif // __FACE_AI_H__