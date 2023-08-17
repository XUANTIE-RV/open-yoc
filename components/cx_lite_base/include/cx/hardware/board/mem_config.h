/**
 * @file mem_config.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_HARDWARE_BOARD_MEM_CONFIG_H
#define CX_HARDWARE_BOARD_MEM_CONFIG_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <semaphore.h>

namespace cx {
namespace board {
/**
 * Mem file example:
 * 
{
	"flash2mem": 
    [
        {
			"name": "singleir0",
			"flash_addr": 123,
			"ram_addr": 888,
			"size": 100
		},
		{
			"name": "singleir2",
			"flash_addr": 234,
			"ram_addr": 999
		}
	]
}
 * 
 */
class MemConfigManager final {
public:
    struct PartitionInfo {
        uint64_t flashAddr;
        uint64_t ramAddr;
        uint32_t size;
    };

public:
    static MemConfigManager *GetInstance(void) {
        return obj;
    }

    using LoadFlash2MemCb = std::function<int (const std::string &partitionName, PartitionInfo &info)>;

    void RegisterFlash2MemCallback(const LoadFlash2MemCb &cb);
    int Parse(const char *configStr);
    int WaitPartitionReady(const std::string partitionName, PartitionInfo &info);

private:
    struct Flash2MemConfig {
        std::string     name;
        PartitionInfo   info;
        bool            valid;
        int             ret;
        sem_t           sem;
        bool            readyFlag;
    };

    static void *Task(void *arg);

    MemConfigManager() = default;
    ~MemConfigManager() = default;
    MemConfigManager(const MemConfigManager &) = delete;

    LoadFlash2MemCb                 mFlash2MemCb;
    std::vector<Flash2MemConfig>    mConfigs;
    static MemConfigManager         *obj;

};


}
}



#endif /* CX_HARDWARE_BOARD_MEM_CONFIG_H */

