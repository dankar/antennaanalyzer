#include "main.h"
#include "serial.h"
#include "arm_math.h"
#include "measure.h"
#include "adc.h"
#include "gui.h"
#include "display.h"
#include "dialogs/menus.h"
#include "dialogs/vswr_graph.h"


#ifdef __SOFTFP__
#error "No fpu"
#endif

extern "C" int real_main()
{
    gui_init();
    //HAL_Delay(2000);
    g_impedance_tester.init();
    //serial_print("Tjo\r\n");
    gui_show(GET_DIALOG_REF(gui_main_menu));
    uint32_t last_print = HAL_GetTick();
    while(1)
    {
        uint32_t time = HAL_GetTick();
        if(is_gui_enabled())
        {
            lv_task_handler();
        }
        HAL_Delay(1);
        if(time > (last_print + 1000))
        {
            last_print = time;
            HAL_GPIO_TogglePin(STATUS_LED1_GPIO_Port, STATUS_LED1_Pin);
        }
        gui_tick();
    }

    return 1;
}
