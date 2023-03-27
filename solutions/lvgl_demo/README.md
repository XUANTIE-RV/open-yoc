# 概述
lvgl_demo是基于lvgl组件的示例程序。包含有lgvl官方提供的lv_demo_stress，lv_demo_widgets，lv_demo_benchmark，lv_demo_music 四个示例程序。
默认程序使用了lv_demo_music播放器示例程序。可以通过修改app_main.c 文件来切换示例程序。

# 切换Demo

通过如下修改app_main.c的Demos程序，可以切换不同的Demos
```
void lvgl_ui_task_run(void)
{
    aos_task_t lvgl_demo_task;
    aos_task_t timer_task;

    aos_mutex_new(&lvgl_mutex);

    LOGD(TAG, "lvgl demo build time: %s, %s\r\n", __DATE__, __TIME__);

    /* Init lvgl*/
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    /* Demos */
    // lv_demo_stress();
    // lv_demo_widgets();
    // lv_demo_benchmark();
    lv_demo_music();

    aos_task_new_ext(&lvgl_demo_task, "lvgl-ui-task", lvgl_ui_task, NULL, CONFIG_UI_TASK_STACK_SIZE,
                     AOS_DEFAULT_APP_PRI);

    aos_task_new_ext(&timer_task, "lvgl-timer-task", lvgl_timer_task, NULL, CONFIG_UI_TASK_STACK_SIZE,
                     AOS_DEFAULT_APP_PRI - 1);
}
```
