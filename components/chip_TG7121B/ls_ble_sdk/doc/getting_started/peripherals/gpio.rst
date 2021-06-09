GPIO
========

本节介绍GPIO输入输出和中断配置。GPIO操作API详见 ``io_config.h`` 。

每一根GPIO都可以在 **普通输入输出** 和 **外设功能** 两种工作模式之间切换。

普通输入输出模式
----------------

输出高低电平    

.. code ::
    
    io_cfg_output(PB00); //设置PB00为输出模式
    io_write_pin(PB00,1); //设置PB00输出高电平
    io_write_pin(PB00,0); //设置PB00输出低电平
    io_set_pin(PB00); //设置PB00输出高电平
    io_clr_pin(PB00); //设置PB00输出低电平
    io_toggle_pin(PB00); //翻转PB00输出电平

读取输入电平

.. code ::

    io_cfg_input(PB01); //设置PB01为输入模式
    uint8_t val = io_read_pin(PB01); //读取PB01电平，保存到val变量

配置上升、下降沿中断

.. code ::
    
    io_cfg_input(PA00); //设置PA00为输入模式
    io_exti_config(PA00,INT_EDGE_RISING); //配置PA00中断，上升沿触发
    io_exti_enable(PA00,true); //使能PA00中断
    io_cfg_input(PB11); //设置PB11为输入模式
    io_exti_config(PB11,INT_EDGE_FALLING); //配置PB11中断，下降沿触发
    io_exti_enable(PB11,true); //使能PB11中断

    void io_exti_callback(uint8_t pin) // override io_exti_callback
    {
        switch(pin)
        {
        case PA00:
            // do something
        break;
        case PB11:
            // do something
        break;
        default:
        break;
        }
    }
 

.. note ::

    在LE501X系统中，只有PA00、PA07、PB11、PB15四个IO可以在任意状态下触发中断，其余IO只能在CPU工作状态下触发中断，不能在BLE休眠状态下触发中断。
    
    BLE处于广播或者连接时，系统根据广播、连接的间隔定期自动休眠、唤醒，所以这种应用场景，建议使用上述四根IO作为中断。

配置内部上、下拉：

.. code ::

    io_pull_write(PA04,IO_PULL_UP); //设置PA04内部上拉
    io_pull_write(PA04,IO_PULL_DOWN); //设置PA04内部下拉
    io_pull_type_t pull = io_pull_read(PA04); //读取PA04内部上下拉状态,保存到pull变量
    io_pull_write(PA04,IO_PULL_DISABLE); //禁用PA04内部上下拉

外设功能模式
-------------

具体配置参考各外设文档

:ref:`uart_ref` 




