/**
 * @file lv_port_disp_templ.c
 *
 */

#include <stdbool.h>
#include <assert.h>
#include <devices/display.h>
#include <ulog/ulog.h>

#include "../../lvgl.h"

static rvm_dev_t *    disp_dev;
static rvm_hal_display_info_t disp_info;
static void           disp_init(void);

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
static void disp_event(rvm_dev_t *dev, int event_id, void *priv);

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    static lv_disp_drv_t      disp_drv; /*Descriptor of a display driver*/
    static lv_disp_draw_buf_t draw_buf_dsc;
    lv_disp_drv_init(&disp_drv); /*Basic initialization*/

    if (disp_info.supported_feature & DISPLAY_FEATURE_ONE_FB) {
        lv_disp_draw_buf_init(&draw_buf_dsc, disp_info.smem_start[0], NULL,
                              disp_info.x_res * disp_info.y_res); /*Initialize the display buffer*/

        disp_drv.full_refresh = 1;
    } else if (disp_info.supported_feature & DISPLAY_FEATURE_DOUBLE_FB) {
        lv_disp_draw_buf_init(&draw_buf_dsc, disp_info.smem_start[0],
                              (uint8_t *)disp_info.smem_start[1],
                              disp_info.x_res * disp_info.y_res); /*Initialize the display buffer*/
        disp_drv.full_refresh = 1;
    } else {
        /** TODO: support display_write_area_async display_write_area */
        assert(0);
    }

    rvm_hal_display_set_event(disp_dev, disp_event, &disp_drv);

    /*Set the resolution of the display*/
    disp_drv.hor_res = disp_info.x_res;
    disp_drv.ver_res = disp_info.y_res;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc;
    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    // disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void disp_event(rvm_dev_t *dev, int event_id, void *priv)
{
    lv_disp_drv_t *drv = (lv_disp_drv_t*)priv;

    if (event_id == DISPLAY_EVENT_FLUSH_DONE) {
        /*IMPORTANT!!!
        *Inform the graphics library that you are ready with the flushing*/
        lv_disp_flush_ready(drv);
    }
}

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    disp_dev = rvm_hal_display_open("disp");
    assert(disp_dev);

    rvm_hal_display_get_info(disp_dev, &disp_info);

    /** assert here cause display pixel format mismatch */
    assert(disp_info.bits_per_pixel == LV_COLOR_DEPTH);
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    static int first = 0;

    rvm_hal_display_pan_display(disp_dev);

    if (first == 0) {
        LOGI("lvgl", "first frame out");
        first = 1;
    }
}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
// static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}
