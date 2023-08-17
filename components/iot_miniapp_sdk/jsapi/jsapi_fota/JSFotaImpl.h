#pragma once
#include <string>
#include <yoc/fota.h>
#include <yoc/netio.h>
#include <yoc/sysinfo.h>
#include <cJSON.h>
#include "jqutil_v2/jqutil.h"

using namespace jqutil_v2;



namespace aiot
{
typedef struct FVersionInof {
    int code;
    std::string curversion;
    std::string newversion;
    std::string deviceid;
    std::string model;
    std::string local_changelog;
    std::string changelog;
    std::string msg;
    inline Bson toBson()
    {
        Bson::object result;
        result["code"] = code;
        result["curversion"] = curversion;
        result["newversion"] = newversion;
        result["deviceid"] = deviceid;
        result["model"] = model;
        result["local_changelog"] = local_changelog;
        result["changelog"] = changelog;
        result["msg"] = msg;
        return result;
    }
} FotaVersion;

typedef struct FDownloadInfo {
    int code;
    int cur_size;
    int total_size;
    int percent;
    int spend;
    std::string msg;
    inline Bson toBson()
    {
        Bson::object result;
        result["code"] = code;
        result["cur_size"] = cur_size;
        result["total_size"] = total_size;
        result["percent"] = percent;
        result["spend"] = spend;
        result["msg"] = msg;
        return result;
    }
} FotaDownload;

typedef struct Fota_Status {
    std::string status;
} FotaStatus;


typedef struct Fend{
    int code;
    std::string msg;
    inline Bson toBson()
    {
        Bson::object result;
        result["code"] = code;
        result["msg"] = msg;
        return result;
    }
}FotaEnd;

typedef struct FRestart{
    int code;
    std::string msg;
    inline Bson toBson()
    {
        Bson::object result;
        result["code"] = code;
        result["msg"] = msg;
        return result;
    }
}FotaRestart;

/* JS方法实现 */
class JSFotaImpl
{
public:
    void init(JQuick::sp<JQPublishObject> pub);
    int start(void);
    int stop(void);
    std::string getState(void);
    int versionCheck(void);
    int download(void);
    int restart(int delayms);

private:
    JQuick::sp<JQPublishObject> _pub;
};

/* JS事件定义 */
static std::string JS_FOTA_EVENT_READY = "ready";
static std::string JS_FOTA_EVENT_VERSION = "version";
static std::string JS_FOTA_EVENT_DOWNLOAD = "download";
static std::string JS_FOTA_EVENT_END = "end";
static std::string JS_FOTA_EVENT_RESTART = "restart";

}  // namespace aiot
