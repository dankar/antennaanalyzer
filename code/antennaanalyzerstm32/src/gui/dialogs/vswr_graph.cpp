#include "vswr_graph.h"
#include "adc.h"
#include "dialogs/menus.h"
#include "measure.h"
#include "ranges.h"
#include "display.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "xtp2046.h"
#include "pixels.h"
#include "util/gcvt.h"

#define LCD_WIDTH 320.0
#define LCD_HEIGHT 240.0

#define NUM_STEPS 256

#define BOTTOM_MARGIN 30.0
#define LEFT_MARGIN 25.0
#define RIGHT_MARGIN 42.0
#define TOP_MARGIN 20.0

#define MAX_VSWR 6.0
#define MAX_R 250.0
#define MAX_PHASE 180.0

#define VSWR_COLOR YELLOW
#define R_COLOR GREEN
#define PHASE_COLOR RED

uint16_t get_vswr_x(float vswr)
{
    if(vswr > MAX_VSWR)
    {
        vswr = MAX_VSWR;
    }

    vswr -= 1.0;

    return (uint16_t)(LCD_HEIGHT - BOTTOM_MARGIN - (LCD_HEIGHT - TOP_MARGIN - BOTTOM_MARGIN) / (MAX_VSWR-1.0) * vswr);
}

uint16_t get_r_x(float r)
{
    return (uint16_t)(get_vswr_x(1.0) - (LCD_HEIGHT - TOP_MARGIN - BOTTOM_MARGIN) / MAX_R * r);
}

uint16_t get_phase_x(float phase)
{
    phase = phase + 90.0;
    return (uint16_t)(get_vswr_x(1.0) - (LCD_HEIGHT - TOP_MARGIN - BOTTOM_MARGIN) / MAX_PHASE * phase);
}

uint16_t get_frequency_y(uint32_t frequency)
{
    range_t range = g_gui_ranges.get_current_range();
    if(frequency < range.start_frequency)
    {
        return -1;
    }
    if(frequency > range.end_frequency)
    {
        return -1;
    }

    float start = LEFT_MARGIN;
    float end = LCD_WIDTH-RIGHT_MARGIN;
    
    frequency = frequency - range.start_frequency;
    float coeff = float(frequency) / float(range.end_frequency - range.start_frequency);

    return lerp(start, end, coeff);
}

void gui_vswr_graph::enter()
{
    disable_gui();
    reset();
}
void gui_vswr_graph::exit()
{
    enable_gui();
}
char result[256];
void gui_vswr_graph::reset()
{
    m_current_step = 0;
    m_max_steps = 128;
    m_previous_vswr = 0;
    m_previous_r = 0;
    m_previous_phase = 0;
    ILI9341_Fill_Screen(BLACK);

    float start = LEFT_MARGIN;
    float end = LCD_WIDTH-RIGHT_MARGIN;
    range_t range = g_gui_ranges.get_current_range();

    for(uint16_t i = 0; i < g_num_ranges - 1; i++)
    {
        range_t all_range = g_ranges[i];
        ILI9341_Draw_Filled_Rectangle_Coord(get_frequency_y(all_range.start_frequency), TOP_MARGIN, get_frequency_y(all_range.end_frequency), LCD_HEIGHT-BOTTOM_MARGIN, DARKGREY);
    }

    for(uint16_t i = 0; i < 5; i++)
    {
        uint32_t freq = range.start_frequency + (range.end_frequency - range.start_frequency) / 4 * i;
        
        gcvt((float(freq)/1000000.0f), 4, result);
        uint16_t x = (uint16_t)(start + (end-start) / 4.0 * (float)i);
        ILI9341_Draw_Text(result, x - 20, (uint16_t)(LCD_HEIGHT - BOTTOM_MARGIN + 20), YELLOW, 1);
        ILI9341_Draw_Vertical_Line(x, TOP_MARGIN, LCD_HEIGHT-TOP_MARGIN-BOTTOM_MARGIN, LIGHTGREY);
    }

    ILI9341_Draw_Text("1.0", 1, get_vswr_x(1.0) + 4.0, VSWR_COLOR, 1);
    ILI9341_Draw_Text("1.5", 1, get_vswr_x(1.5) + 4.0, VSWR_COLOR, 1);
    ILI9341_Draw_Text("2.0", 1, get_vswr_x(2.0) + 4.0, VSWR_COLOR, 1);
    ILI9341_Draw_Text("2.5", 1, get_vswr_x(2.5) + 4.0, VSWR_COLOR, 1);
    ILI9341_Draw_Text("3.0", 1, get_vswr_x(3.0) + 4.0, VSWR_COLOR, 1);
    ILI9341_Draw_Text("3.5", 1, get_vswr_x(3.5) + 4.0, VSWR_COLOR, 1);
    ILI9341_Draw_Text("4.0", 1, get_vswr_x(4.0) + 4.0, VSWR_COLOR, 1);
    ILI9341_Draw_Text("4.5", 1, get_vswr_x(4.5) + 4.0, VSWR_COLOR, 1);
    ILI9341_Draw_Text("5.0", 1, get_vswr_x(5.0) + 4.0, VSWR_COLOR, 1);
    ILI9341_Draw_Text("5.5", 1, get_vswr_x(5.5) + 4.0, VSWR_COLOR, 1);
    ILI9341_Draw_Text("6.0", 1, get_vswr_x(6.0) + 4.0, VSWR_COLOR, 1);

    ILI9341_Draw_Text("0", LCD_WIDTH-40, get_r_x(0.0) + 4.0, R_COLOR, 1);
    ILI9341_Draw_Text("25", LCD_WIDTH-40, get_r_x(25.0) + 4.0, R_COLOR, 1);
    ILI9341_Draw_Text("50", LCD_WIDTH-40, get_r_x(50.0) + 4.0, R_COLOR, 1);
    ILI9341_Draw_Text("75", LCD_WIDTH-40, get_r_x(75.0) + 4.0, R_COLOR, 1);
    ILI9341_Draw_Text("100", LCD_WIDTH-40, get_r_x(100.0) + 4.0, R_COLOR, 1);
    ILI9341_Draw_Text("125", LCD_WIDTH-40, get_r_x(125.0) + 4.0, R_COLOR, 1);
    ILI9341_Draw_Text("150", LCD_WIDTH-40, get_r_x(150.0) + 4.0, R_COLOR, 1);
    ILI9341_Draw_Text("175", LCD_WIDTH-40, get_r_x(175.0) + 4.0, R_COLOR, 1);
    ILI9341_Draw_Text("200", LCD_WIDTH-40, get_r_x(200.0) + 4.0, R_COLOR, 1);
    ILI9341_Draw_Text("225", LCD_WIDTH-40, get_r_x(225.0) + 4.0, R_COLOR, 1);
    ILI9341_Draw_Text("250", LCD_WIDTH-40, get_r_x(250.0) + 4.0, R_COLOR, 1);

    ILI9341_Draw_Text("-90", LCD_WIDTH-20, get_phase_x(-90.0) + 4.0, PHASE_COLOR, 1);
    ILI9341_Draw_Text("-72", LCD_WIDTH-20, get_phase_x(-72.0) + 4.0, PHASE_COLOR, 1);
    ILI9341_Draw_Text("-54", LCD_WIDTH-20, get_phase_x(-54.0) + 4.0, PHASE_COLOR, 1);
    ILI9341_Draw_Text("-36", LCD_WIDTH-20, get_phase_x(-36.0) + 4.0, PHASE_COLOR, 1);
    ILI9341_Draw_Text("-18", LCD_WIDTH-20, get_phase_x(-18.0) + 4.0, PHASE_COLOR, 1);
    ILI9341_Draw_Text("  0", LCD_WIDTH-20, get_phase_x(0.0) + 4.0, PHASE_COLOR, 1);
    ILI9341_Draw_Text(" 18", LCD_WIDTH-20, get_phase_x(18.0) + 4.0, PHASE_COLOR, 1);
    ILI9341_Draw_Text(" 36", LCD_WIDTH-20, get_phase_x(36.0) + 4.0, PHASE_COLOR, 1);
    ILI9341_Draw_Text(" 54", LCD_WIDTH-20, get_phase_x(54.0) + 4.0, PHASE_COLOR, 1);
    ILI9341_Draw_Text(" 72", LCD_WIDTH-20, get_phase_x(72.0) + 4.0, PHASE_COLOR, 1);
    ILI9341_Draw_Text(" 90", LCD_WIDTH-20, get_phase_x(90.0) + 4.0, PHASE_COLOR, 1);

    ILI9341_Draw_Horizontal_Line(LEFT_MARGIN, get_vswr_x(1.0), LCD_WIDTH-LEFT_MARGIN-RIGHT_MARGIN, LIGHTGREY);
    ILI9341_Draw_Horizontal_Line(LEFT_MARGIN, get_vswr_x(1.5), LCD_WIDTH-LEFT_MARGIN-RIGHT_MARGIN, LIGHTGREY);
    ILI9341_Draw_Horizontal_Line(LEFT_MARGIN, get_vswr_x(2.0), LCD_WIDTH-LEFT_MARGIN-RIGHT_MARGIN, LIGHTGREY);
    ILI9341_Draw_Horizontal_Line(LEFT_MARGIN, get_vswr_x(2.5), LCD_WIDTH-LEFT_MARGIN-RIGHT_MARGIN, LIGHTGREY);
    ILI9341_Draw_Horizontal_Line(LEFT_MARGIN, get_vswr_x(3.0), LCD_WIDTH-LEFT_MARGIN-RIGHT_MARGIN, LIGHTGREY);
    ILI9341_Draw_Horizontal_Line(LEFT_MARGIN, get_vswr_x(3.5), LCD_WIDTH-LEFT_MARGIN-RIGHT_MARGIN, LIGHTGREY);
    ILI9341_Draw_Horizontal_Line(LEFT_MARGIN, get_vswr_x(4.0), LCD_WIDTH-LEFT_MARGIN-RIGHT_MARGIN, LIGHTGREY);
    ILI9341_Draw_Horizontal_Line(LEFT_MARGIN, get_vswr_x(4.5), LCD_WIDTH-LEFT_MARGIN-RIGHT_MARGIN, LIGHTGREY);
    ILI9341_Draw_Horizontal_Line(LEFT_MARGIN, get_vswr_x(5.0), LCD_WIDTH-LEFT_MARGIN-RIGHT_MARGIN, LIGHTGREY);
    ILI9341_Draw_Horizontal_Line(LEFT_MARGIN, get_vswr_x(5.5), LCD_WIDTH-LEFT_MARGIN-RIGHT_MARGIN, LIGHTGREY);
    ILI9341_Draw_Horizontal_Line(LEFT_MARGIN, get_vswr_x(6.0), LCD_WIDTH-LEFT_MARGIN-RIGHT_MARGIN, LIGHTGREY);
}

void gui_vswr_graph::init()
{
    
}

void gui_vswr_graph::tick()
{
    touch_t tp = xtp2046_update();

    if(tp.touched)
    {
        gui_show(GET_DIALOG_REF(gui_main_menu));
    }

    if(m_current_step < m_max_steps)
    {
        range_t range = g_gui_ranges.get_current_range();
        uint32_t freq = range.start_frequency + (range.end_frequency - range.start_frequency) / m_max_steps * m_current_step;
        g_impedance_tester.make_measurement(freq);

        float vswr = g_impedance_tester.get_vswr();
        complex_t Z = g_impedance_tester.get_impedance();
        float r = Z.magnitude();
        float phase = atan2f(Z.imag, Z.real) * 90.0/3.1415926;

        if(m_current_step != 0)
        {
            uint16_t previous_x = LEFT_MARGIN + (LCD_WIDTH-LEFT_MARGIN-RIGHT_MARGIN) / m_max_steps * (m_current_step-1);
            uint16_t current_x = LEFT_MARGIN + (LCD_WIDTH-LEFT_MARGIN-RIGHT_MARGIN) / m_max_steps * (m_current_step);
            draw_line(previous_x, get_vswr_x(m_previous_vswr), current_x, get_vswr_x(vswr), YELLOW);

            draw_line(previous_x, get_r_x(m_previous_r), current_x, get_r_x(r), GREEN);
            draw_line(previous_x, get_phase_x(m_previous_phase), current_x, get_phase_x(phase), RED);
        }
    
        m_current_step++;
        m_previous_vswr = vswr;
        m_previous_r = r;
        m_previous_phase = phase;
    }
}

IMPLEMENT_DIALOG(gui_vswr_graph);