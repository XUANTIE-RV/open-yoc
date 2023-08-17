#include "JSPowerImpl.h"
#include  <ulog/ulog.h>
#include <aos/aos.h>
#include <aos/kv.h>
#include "cpu_pwr_config.h"

#define TAG "JSPower"
#define AUTOHIBER "POWER_autoHibernate"
#define TIMEOUT "POWER_timeout"


namespace aiot
{
static  PowerManageInfo powerManageInfo;

void JSPowerImpl::init(JQuick::sp< JQPublishObject > pub)
{
    _pub = pub;
    int ret = 0;
    int temp;
    LOGD(TAG, "power init ....");
    // TODO: 初始化电源管理状态
    // 检查是否设置自动休眠,默认关闭
    ret = aos_kv_getint(AUTOHIBER, &temp);
    if (0 == ret)
    {
        powerManageInfo.autoHibernate = (bool)temp;
    }else {
        aos_kv_setint(AUTOHIBER, 0);
    }
    // 检查设置的自动休眠时间
    ret = aos_kv_getint(TIMEOUT,&temp);
    if (0 == ret)
    {
        // 默认自动休眠时间 未300秒
        aos_kv_setint(TIMEOUT, 600);
        aos_kv_getint(TIMEOUT,&temp);
        powerManageInfo.hibernateTimeout = temp;
    }else {
        aos_kv_setint(TIMEOUT, 600);
    }

}
/**
 * 获取电源管理状态
 * @param cb
 */
void JSPowerImpl::getInfo(PowerCallback cb)
{
    // TODO  获取信息并回传
    //  MOCK data
    // powerManageInfo.autoHibernate = false;
    // powerManageInfo.hibernateTimeout = 20000;
    // 默认电量 66 ，电池状态未充电
    powerManageInfo.batteryPercent = 66;
    powerManageInfo.isCharging = false;
    cb(powerManageInfo);
}

/**
 * 设置是否启用超时自动休眠
 * @param isOn  true-启用，false-禁用
 */
void JSPowerImpl::setAutoHibernate(bool isOn)
{
    // TODO setAutoHibernate
    aos_kv_setint(AUTOHIBER, isOn);
    powerManageInfo.autoHibernate = isOn;
    
}

/**
 * 设置自动休眠的闲置时间
 * @param time 单位：s
 */
void JSPowerImpl::setHibernateTime(int time)
{
    LOGD(TAG, "JSPowerImpl setHibernateTime time = %d", time);
    aos_kv_setint(TIMEOUT, time);
    powerManageInfo.hibernateTimeout = time;
}

/**
 * 关机
 */
void JSPowerImpl::shutdown()
{
    LOGD(TAG, "JSPowerImpl shutdown");
    // TODO shutdown
}

/**
 * 重启
 */
void JSPowerImpl::reboot()
{
    // TODO reboot
    LOGD(TAG, "power reboot \n");
    aos_reboot();
}
}  // namespace aiot