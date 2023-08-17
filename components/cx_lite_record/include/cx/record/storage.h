/**
 * @file storage.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_RECORD_STORAGE_H
#define CX_RECORD_STORAGE_H

#include <memory>
#include <string>
#include <cx/common/sorted_list.h>
#include <cx/common/frame.h>
#include <tmedia_core/entity/filesink/filesink.h>
#include <pthread.h>
#include <semaphore.h>

namespace cx {
namespace record {


struct FilePath {
    FilePath() : valid(false), isVid(false) {};
    FilePath(const std::string &rootPath, const std::string &folderName, const std::string &fileName);
    FilePath(const std::string &filePath);

    bool IsValid() const;
    bool IsVideo() const;

    /**
     * @param[in] timestamp   int the form YYYYMMDDhhmmss, e.g. 20211030083510
     */ 
    bool OlderThan(const std::string &timestamp) const;
    bool NewerThan(const std::string &timestamp) const;
    bool SameAs(const std::string &timestamp) const;

    static bool CompareFileOlderThan(const FilePath &a, const FilePath &b);

    std::string GetFullPath() const {
        return rootPath + '/' + folderName + '/' + fileName;
    }

    std::string rootPath;
    std::string folderName;
    std::string fileName;
    std::string timestamp;

    bool valid;
    bool isVid;

private:
    void CheckFileName(const std::string &fileName);
};

using FileList = SortedList<FilePath, decltype(FilePath::CompareFileOlderThan) *>;

class FolderManager {
public:
    FolderManager(const std::string &recorderPath);
    ~FolderManager();

    int GetTotalSize(uint32_t &size);
    int GetVidSize(uint32_t &size);
    int GetPicSize(uint32_t &size);

    void ReCalc(uint32_t &size);    // TODO

    /**
     * @param[in] folderName folder name
     * @param[in] deltaSize size added to the folder, 0 for recaculate
     */ 
    int UpdateFolerInfo(bool isVidFolder, const std::string &folderName, int deltaSize);
    
    int DeleteFiles(const vector<FilePath> &files);

    /**
     * @param[in] beginTime int the form YYYYMMDDhhmmss, e.g. 20211030083510
     * @param[in] endTime   int the form YYYYMMDDhhmmss, e.g. 20211030083510
     * @param[out] flist    file list of the matched files
     */ 
    int SearchVideoFiles(const std::string &beginTime, const std::string &endTime, FileList &flist) const;
    int SearchSnapFiles(const std::string &beginTime, const std::string &endTime, FileList &flist) const;

    int AddVideoFile(const std::string &folderName, const std::string &fileName);
    int AddSnapFile(const std::string &folderName, const std::string &fileName);

    /* return deleted size */
    uint32_t DeleteOldest(uint32_t size);
    
private:
    struct FolderInfo {
        uint32_t    size;
        std::string name;
        uint32_t    timeStamp;     // converted directly from name;

        /**
         * @param[in] timestamp   int the form YYYYMMDDhhmmss, e.g. 20211030083510
         */ 
        bool OlderThan(const std::string &timestamp) const;

        bool SameAs(const std::string &timestamp) const;
    };

    static bool CompareFolderOlderThan(const FolderInfo &a, const FolderInfo &b);

    using FolderList = SortedList<FolderInfo, decltype(CompareFolderOlderThan) *>;
    int GetFolderSize(const std::string &path, FolderList &flist, uint32_t &size);
    int AddNewFolder(const std::string &path, const std::string &name, FolderList &flist, uint32_t *size);
    int SearchFiles(const std::string &path, const std::string &suffix, const FolderList &searchedFolders, const std::string &beginTime, const std::string &endTime, FileList &flist) const;
    int AddFile(bool isVidFolder, const std::string &folderName, const std::string &fileName);

    const std::string mRecorderPath;
    const std::string mVidPath;
    const std::string mPicPath;

    /* folder list from old to new  */
    FolderList mVidFolderList;
    FolderList mPicFolderList;

    pthread_mutex_t mLock;

    bool inited;
};


class RecorderStorage {
public:
    RecorderStorage(const std::string rootPath, uint32_t maxSizeKB, uint32_t segmentSize, uint8_t recycleLevel);
    ~RecorderStorage();

    // recalculate the used size
    int GetSpace(uint32_t *used);

    // int ReleaseSpace(uint32_t size);
    int VideoStart();
    int VideoSave(shared_ptr<CxPacket> &pkt);
    int VideoStop(string &file_path, string &file_name);

    int SnapSave(shared_ptr<CxVideoPacket> &pkt, string &file_path, string &file_name);

    int SearchVideoFiles(const std::string &beginTime, const std::string &endTime, FileList &flist) const {
        return mFolderManager->SearchVideoFiles(beginTime, endTime, flist);
    }

    int SearchSnapFiles(const std::string &beginTime, const std::string &endTime, FileList &flist) const {
        return mFolderManager->SearchSnapFiles(beginTime, endTime, flist);
    }

    int DeleteVideoFiles(const vector<FilePath> &files) {
        return mFolderManager->DeleteFiles(files);
    }

    int DeleteSnapFiles(const vector<FilePath> &files) {
        return mFolderManager->DeleteFiles(files);
    }

private:
    const std::string   mRootPath;
    const uint32_t      mTotalSpace;
    const uint32_t      mSegmentSize;
    const uint8_t       mRecycleLevel;
    uint32_t            mUsedSpace;

    FilePath            mVideoFilePath;
    FilePath            mSnapFilePath;

    std::unique_ptr<FolderManager> mFolderManager;

    std::unique_ptr<TMFileSink>         mVideoSink;
    std::unique_ptr<TMFileSink>         mSnapSink;

    /* for space recollection */
    bool                mStorageInited;
    sem_t               mSem;
    bool                mRecyleRunning;
    pthread_t           mTid;
    static void *SpaceRecyclingThread(void *arg);
};

}
} 



#endif /* CX_RECORD_STORAGE_H */

