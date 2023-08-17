/**
 * @file aud_manager.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_SROUCE_AUD_MANAGER_H
#define CX_SROUCE_AUD_MANAGER_H

#include <cx/source/aud_channel.h>

namespace cx {
namespace source {

class AudSrcManager final {
public:
    int AddChannelConfig(const AudChannelConfig &config);
    shared_ptr<AudChannel> CreateChannel(int chanId);

    // factory method
    static AudSrcManager *GetInstance(void) {
        return pObj;
    }

private:

    AudSrcManager();
    ~AudSrcManager();

    std::unique_ptr<std::vector<std::pair<AudChannelConfig, AudChannelPtr>>> mChannels;
    static void     *Task(void *args);
    static AudSrcManager *pObj;
};

}
} 


#endif /* CX_SROUCE_AUD_MANAGER_H */

