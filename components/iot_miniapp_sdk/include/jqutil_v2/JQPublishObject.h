#include "jqutil_v2/JQObjectTemplate.h"
#include "jqutil_v2/JQFunctionTemplate.h"
#include <string>
#include <vector>
#include <map>

namespace jqutil_v2 {

typedef enum {
    JQ_PUBLISH_TYPE_AUTO,
    JQ_PUBLISH_TYPE_ASYNC,
    JQ_PUBLISH_TYPE_SYNC,
} JQPublishType;

class JQPublishObject: public JQBaseObject {
public:
    JQPublishObject();
    virtual void OnInit();
    static void InitTpl(JQFunctionTemplateRef &tpl);
    static void InitTpl(JQObjectTemplateRef &tpl);

    void publishJSON(const std::string &topic, const std::string &json,
                     JQPublishType pubType=JQ_PUBLISH_TYPE_AUTO);
    void publish(const std::string &topic, const Bson &bson,
                 JQPublishType pubType=JQ_PUBLISH_TYPE_AUTO);
    virtual void onSubscribe(const char* topic);
    virtual void onUnsubscribe(const char* topic);

private:
    // pub/sub callback
    void _SubscribeTopic(JQFunctionInfo &info);
    void _UnsubscribeTopic(JQFunctionInfo &info);
    void _OnPublishJSON(const std::string &topic, const std::string &json);
    void _OnPublish(const std::string &topic, const Bson &json);

    // topic to callbacks as <token, callback> list
    std::map<std::string/*topic*/, std::vector<std::pair<uint32_t, JSValue> > > _topicCallbacksMap;
    uint32_t _pubCbTokenId;
};

}  // namespace jqutil_v2