/*
 *  Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/*******************************************************
 * @file    dw_timer.c
 * @brief   source file for timer csi driver
 * @version V1.0
 * @date    23. Sep 2020
 * ******************************************************/

#include <csi_config.h>
#include <drv/timer.h>
#include <drv/irq.h>

/**
  \brief       Timer interrupt handling function
  \param[in]   arg    Callback function member variables
  \return      None
*/
void dw_timer_irq_handler(void *arg)
{

    ///< TODO：获取中断状态
    ///< TODO：根据中断状态清除中断
    ///< TODO：根据中断状态执行用户回调函数
    
}
/**
  \brief       Initialize TIMER Interface. 1. Initializes the resources needed for the TIMER interface 2.registers callback function
  \param[in]   timer    handle timer handle to operate
  \param[in]   idx      timer index
  \return      error code \ref csi_error_t
*/
csi_error_t csi_timer_init(csi_timer_t *timer, uint32_t idx)
{
    CSI_PARAM_CHK(timer, CSI_ERROR);

    csi_error_t ret = CSI_OK;

    ///< 获取中断号、基地址等相关信息
    if (0 == target_get(DEV_DW_TIMER_TAG, idx, &timer->dev)) {

        ///< TODO：复位加载值寄存器
        ///< TODO：复位控制寄存器

    } else {
        ret = CSI_ERROR;
    }

    return ret;
}
/**
  \brief       De-initialize TIMER Interface. stops operation and releases the software resources used by the interface
  \param[in]   timer    handle timer handle to operate
  \return      None
*/
void csi_timer_uninit(csi_timer_t *timer)
{
    CSI_PARAM_CHK_NORETVAL(timer);

    ///< TODO：复位加载值寄存器
    ///< TODO：复位控制寄存器

}
/**
  \brief       Start timer
  \param[in]   timer         handle timer handle to operate
  \param[in]   timeout_us    the timeout for timer
  \return      error code \ref csi_error_t
*/
csi_error_t csi_timer_start(csi_timer_t *timer, uint32_t timeout_us)
{
    CSI_PARAM_CHK(timer, CSI_ERROR);
    CSI_PARAM_CHK(timeout_us, CSI_ERROR);
    csi_error_t ret = CSI_OK;

    ///< TODO：获取TIMER的工作频率
    ///< TODO：根据TIMER的工作频率和timeout_us得出加载值寄存器应该配的值
    ///< TODO：配置加载值寄存器设置定时时间

    ///< TODO：关闭TIMER
    ///< TODO：打开TIMER
    ///< TODO：打开TIMER的中断

    return ret;
}
/**
  \brief       Stop timer
  \param[in]   timer    handle timer handle to operate
  \return      None
*/
void csi_timer_stop(csi_timer_t *timer)
{
    CSI_PARAM_CHK_NORETVAL(timer);

    ///< TODO：关闭TIMER的中断
    ///< TODO：关闭TIMER

}
/**
  \brief       Get timer remaining value
  \param[in]   timer    handle timer handle to operate
  \return      the remaining value
*/
uint32_t csi_timer_get_remaining_value(csi_timer_t *timer)
{
    CSI_PARAM_CHK(timer, 0U);
    uint32_t cur_value = 0U;

    ///< TODO：获取当前值寄存器中的值

    return cur_value;
}
/**
  \brief       Get timer load value
  \param[in]   timer    handle timer handle to operate
  \return      the load value
*/
uint32_t csi_timer_get_load_value(csi_timer_t *timer)
{
    CSI_PARAM_CHK(timer, 0U);
    uint32_t load_val = 0U;

    ///< TODO：获取加载值寄存器中的值

    return load_val;
}
/**
  \brief       Check timer is running
  \param[in]   timer    handle timer handle to operate
  \return      true->running, false->stopped
*/
bool csi_timer_is_running(csi_timer_t *timer)
{
    CSI_PARAM_CHK(timer, false);
    uint32_t status = 0U;

    ///< TODO：获取TIMER的运行状态

    return status;
}

/**
  \brief       Attach the callback handler to timer
  \param[in]   timer       operate handle.
  \param[in]   callback    callback function
  \param[in]   arg         callback's param
  \return      error code \ref csi_error_t
*/
csi_error_t csi_timer_attach_callback(csi_timer_t *timer, void *callback, void *arg)
{
    CSI_PARAM_CHK(timer, CSI_ERROR);

    timer->callback = callback;
    timer->arg = arg;

    ///< 注册TIMER中断服务函数，使能中断控制器对应的中断
    csi_irq_attach((uint32_t)timer->dev.irq_num, &dw_timer_irq_handler, &timer->dev);
    csi_irq_enable((uint32_t)timer->dev.irq_num);

    return CSI_OK;
}

/**
  \brief       Detach the callback handler
  \param[in]   timer    operate handle.
*/
void csi_timer_detach_callback(csi_timer_t *timer)
{
    CSI_PARAM_CHK_NORETVAL(timer);

    timer->callback = NULL;
    timer->arg = NULL;

    ///< 关闭中断控制器对应的中断，注销中断服务函数
    csi_irq_disable((uint32_t)timer->dev.irq_num);
    csi_irq_detach((uint32_t)timer->dev.irq_num);
}

#ifdef CONFIG_PM
csi_error_t dw_timer_pm_action(csi_dev_t *dev, csi_pm_dev_action_t action)
{
    CSI_PARAM_CHK(dev, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    csi_pm_dev_t *pm_dev = &dev->pm_dev;

    switch (action) {
        case PM_DEV_SUSPEND:
            ///< TODO：恢复TIMER寄存器
            break;

        case PM_DEV_RESUME:
            ///< TODO：保存TIMER寄存器
            break;

        default:
            ret = CSI_ERROR;
            break;
    }

    return ret;
}

csi_error_t csi_timer_enable_pm(csi_timer_t *timer)
{
    ///< TODO：注册TIMER低功耗处理函数dw_timer_pm_action
}

void csi_timer_disable_pm(csi_timer_t *timer)
{
    csi_pm_dev_unregister(&timer->dev);
}
#endif

