## PWM通用接口设计特性（针对WJ）

## 设计描述	

​		因该芯片用户指导手册描述跟常规PWM模块，在使用过程中可能会存在一定误区，故此文档对该特性做了详细描述，方便用户快速上手PWM功能。

​		芯片用户手册中PIN复用功能总共有12路PWM通道可做选择，但是PWM控制器下挂了6组PWM发生器，每组发生器下外接了两路PWM，其中偶数通道可做输入捕获或者信号输出，奇数通道只能作为信号输出（如下图），当用户配置偶数通道作为输入捕获时，相同发生器下的另一路奇数通道则会被迫关闭输出功能，故针对该场景对于驱动代码做了**通用性设计**，接口中的通道选择实际根据发生器选择具体的控制通道。

​		比如当用户配置**通道1**作为输入或者输出时，实际对应的是下图中的“**PWM2**”，配置**通道2**时对应的是下图中的“**PWM4**“，以此类推，总共可控通道范围为**0~5**。

![img](./wj_pwm_doc_picture/clip_image001.jpg)

## 示例演示

### 案例一

平台为Danica，配置PWM2信号输出，信号周期100us，低电平时间50us；

```c
void pwm_signel_output(void)
{
    csi_pwm_t pwm_handle;
    
    csi_pwm_init(&pwm_handle, 0);
    csi_pwm_out_config(&pwm_handle, 1, 100, 50, PWM_POLARITY_LOW);
    csi_pwm_out_start(&pwm_handle, 1);
    while(1); ///< use an oscilloscope to test the signel here
    csi_pwm_out_stop(&pwm_handle, 1);
    csi_pwm_uninit(&pwm_handle);
}    
```





