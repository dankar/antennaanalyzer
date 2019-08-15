#include "display.h"
#include "lvgl.h"
#include "ILI9341_STM32_Driver.h"
#include "xtp2046.h"

static void ex_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
    if(is_gui_enabled())
    {
        ILI9341_Blit(x1, y1, x2, y2, (const char*)color_p);
    }
    lv_flush_ready();
}

static void ex_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
    if(is_gui_enabled())
    {
        ILI9341_Blit(x1, y1, x2, y2, (const char*)color_p);
    }
}

static void ex_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2,  lv_color_t color)
{
    if(is_gui_enabled())
    {
        int32_t x;
        int32_t y;
        for(y = y1; y <= y2; y++) {
            for(x = x1; x <= x2; x++) {
                ILI9341_Draw_Pixel(x, y, color.full);
            }
        }
    }
}

static bool ex_tp_read(lv_indev_data_t * data)
{
    touch_t tp = xtp2046_update();

    data->state = tp.touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    data->point.x = tp.x;
    data->point.y = tp.y;

    return false;
}

void display_init()
{
    ILI9341_Init();
    xtp2046_init();
    lv_init();
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.disp_flush = ex_disp_flush;
    disp_drv.disp_fill = ex_disp_fill;
    disp_drv.disp_map = ex_disp_map;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_t indev_drv;                       /*Descriptor of an input device driver*/
    lv_indev_drv_init(&indev_drv);                  /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;         /*The touchpad is pointer type device*/
    indev_drv.read = ex_tp_read;                 /*Library ready your touchpad via this function*/
    lv_indev_drv_register(&indev_drv);              /*Finally register the driver*/

    lv_theme_t * th = lv_theme_material_init(210, NULL);

    /*Set the surent system theme*/
    lv_theme_set_current(th);
}

volatile int g_is_gui_enabled = 1;

void enable_gui()
{
    g_is_gui_enabled = 1;
}
void disable_gui()
{
    g_is_gui_enabled = 0;
}
int is_gui_enabled()
{
    return g_is_gui_enabled;
}