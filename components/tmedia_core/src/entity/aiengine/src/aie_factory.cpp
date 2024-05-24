/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#include <iostream>

#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <tmedia_core/entity/aiengine/aiengine_inc.h>

using namespace std;

TMImageProc *ImageProcFactory::CreateEntity(string class_name)
{
    if (mImageProcClass().size() == 0)
    {
        LOG_E("imageproc register null\n");
        return NULL;
    }
    if (class_name.empty())
    {
        TMImageProc *imageproc = (TMImageProc *)mImageProcClass().begin()->second();
        LOG_I("imageproc auto select backend:%s", mImageProcClass().begin()->first.c_str());
        return imageproc;
    }
    else
    {
        vector<pair<string, class_new_t>>::iterator it;
        for (it = mImageProcClass().begin(); it != mImageProcClass().end(); it++)
        {
            if (it->first == class_name)
            {
                TMImageProc *imageproc = (TMImageProc *)it->second();
                return imageproc;
            }
        }
        LOG_I("imageproc current register:");
        for (it = mImageProcClass().begin(); it != mImageProcClass().end(); it++)
        {
            LOG_I(" %s", it->first.c_str());
        }
        LOG_I("\n");
        return NULL;
    }
}

TMOsd *OSDFactory::CreateEntity(string class_name)
{
    if (mOSDClass().size() == 0)
    {
        LOG_E("osd register null\n");
        return NULL;
    }

    if (class_name.empty())
    {
        TMOsd *osd = (TMOsd *)mOSDClass().begin()->second();
        LOG_I("osd auto select backend:%s\n" << mOSDClass().begin()->first.c_str());
        return osd;
    }
    else
    {
        vector<pair<string, class_new_t>>::iterator it;
        for (it = mOSDClass().begin(); it != mOSDClass().end(); it++)
        {
            if (it->first == class_name)
            {
                TMOsd *osd = (TMOsd *)it->second();
                return osd;
            }
        }
        LOG_I("osd current register:");
        for (it = mOSDClass().begin(); it != mOSDClass().end(); it++)
        {
            LOG_I(" %s", it->first.c_str());
        }
        LOG_I("\n");
        return NULL;
    }
}

TMInterpreter *InterpreterFactory::CreateEntity(string class_name)
{
    if (mInterpreterClass().size() == 0)
    {
        LOG_E("interpreter register null\n");
        return NULL;
    }

    if (class_name.empty())
    {
        TMInterpreter *interpreter = (TMInterpreter *)mInterpreterClass().begin()->second();
        LOG_I("interpreter auto select backend:%s", mInterpreterClass().begin()->first.c_str());
        return interpreter;
    }
    else
    {
        vector<pair<string, class_new_t>>::iterator it;
        for (it = mInterpreterClass().begin(); it != mInterpreterClass().end(); it++)
        {
            if (it->first == class_name)
            {
                TMInterpreter *interpreter = (TMInterpreter *)it->second();
                return interpreter;
            }
        }
        LOG_I("interpreter current register:");
        for (it = mInterpreterClass().begin(); it != mInterpreterClass().end(); it++)
        {
            LOG_I(" %s", it->first.c_str());
        }
        LOG_I("\n");
        return NULL;
    }
}

FCE *FCEFactory::CreateEntity()
{
    return mFCEClass() ? (FCE *)mFCEClass()() : NULL;
}
