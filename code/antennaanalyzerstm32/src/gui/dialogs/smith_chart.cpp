#include "smith_chart.h"
#include "adc.h"
#include "dialogs/menus.h"
#include "measure.h"
#include "pixels.h"
#include "ILI9341_STM32_Driver.h"

complex_t gamma(complex_t z)
{
	complex_t Zo(50.0f, 0.0f);
	return (z - Zo) / (z + Zo);
}

static lv_res_t back_button_callback(lv_obj_t * btn)
{
	g_gui_smith_chart.m_chart_drawn = false;
    gui_show(GET_DIALOG_REF(gui_main_menu));
    return 1;
}

void gui_smith_chart::get_xy_from_gamma(int &x, int &y, const complex_t &gamma)
{
	x = m_mid_x + float(m_radius) * gamma.real;
	y = m_mid_y + float(m_radius) * -gamma.imag;
}

void gui_smith_chart::draw_negative_resistance_circle(float res)
{
	complex_t z(res, 0.0f);
	int x = 0, y = 0;

	get_xy_from_gamma(x, y, gamma(z));

	int circle_radius = (m_mid_x + m_radius - x) / 2;
	int circle_x = 320 - x - circle_radius;
	int circle_y = y;

	draw_limited_circle(circle_x, circle_y, circle_radius, RED, m_mid_x, m_mid_y, m_radius);
}

void gui_smith_chart::draw_negative_impedance_circle(float gamma_y)
{
	int x = 0, y = 0;

	get_xy_from_gamma(x, y, complex_t(0.0f, gamma_y));

	int circle_radius = (m_mid_y - y) / 2;
	int circle_x = 320 - m_mid_x - m_radius;
	int circle_y = m_mid_y - circle_radius;

	draw_limited_circle(circle_x, circle_y, abs(circle_radius), RED, m_mid_x, m_mid_y, m_radius);
}

void gui_smith_chart::draw_resistance_circle(float res)
{
	complex_t z(res, 0.0f);
	int x = 0, y = 0;

	get_xy_from_gamma(x, y, gamma(z));

	int circle_radius = (m_mid_x + m_radius - x) / 2;
	int circle_x = x + circle_radius;
	int circle_y = y;

	draw_limited_circle(circle_x, circle_y, circle_radius, BLUE, m_mid_x, m_mid_y, m_radius);
}

void gui_smith_chart::draw_impedance_circle(float gamma_y)
{
	int x = 0, y = 0;

	get_xy_from_gamma(x, y, complex_t(0.0f, gamma_y));

	int circle_radius = (m_mid_y - y) / 2;
	int circle_x = m_mid_x + m_radius;
	int circle_y = m_mid_y - circle_radius;

	draw_limited_circle(circle_x, circle_y, abs(circle_radius), BLUE, m_mid_x, m_mid_y, m_radius);
}

void gui_smith_chart::draw_smith_chart()
{
	draw_circle(m_mid_x, m_mid_y, m_radius, BLACK);
	draw_line(m_mid_x - m_radius + 1, m_mid_y, m_mid_x + m_radius - 1, m_mid_y, BLACK);
	draw_line(m_mid_x, m_mid_y - m_radius + 1, m_mid_x, m_mid_y + m_radius - 1, BLACK);

	for (float f = 50.0f / 8.0f; f < 1000.0f; f *= 2.0f)
	{
		draw_resistance_circle(f);
		draw_negative_resistance_circle(f);
	}

	for (float f = 0.1f; f < 20.0f; f *= 2.0f)
	{
		draw_impedance_circle(f);
		draw_negative_impedance_circle(f);
	}

	for (float f = -0.1f; f > -20.0f; f *= 2.0f)
	{
		draw_impedance_circle(f);
		draw_negative_impedance_circle(f);
	}
}

void gui_smith_chart::init()
{
    m_scr = lv_page_create(NULL, NULL);
    m_impedance_label = lv_label_create(m_scr, NULL);
    lv_label_set_text(m_impedance_label, "NULL");
    lv_obj_align(m_impedance_label, NULL, LV_ALIGN_CENTER, 0, 100);

    m_back_button = lv_btn_create(m_scr, NULL);
    lv_btn_set_action(m_back_button, LV_BTN_ACTION_CLICK, back_button_callback);
    lv_obj_align(m_back_button, NULL, LV_ALIGN_CENTER, -110, 190);

    /*Create a label on the button (the 'label' variable can be reused)*/
    lv_obj_t *label = lv_label_create(m_back_button, NULL);
    lv_label_set_text(label, "Back");
    m_mid_x = 320 / 2;
    m_mid_y = 240 / 2;
    m_radius = 240 / 3;
    m_chart_drawn = false;
}

void gui_smith_chart::tick()
{
    char test[256];
    complex_t impedance = g_impedance_tester.get_impedance();

    sprintf(test, "%d.%d + j%d.%d", int(impedance.real), int((impedance.real-int(impedance.real))*10.0f), int(impedance.imag), int((impedance.imag-int(impedance.imag))*10.0f));
    lv_label_set_text(m_impedance_label, test);

    //put_pixel(m_mid_x, m_mid_y, BLACK);
    if(!m_chart_drawn)
    {
        draw_smith_chart();
        m_chart_drawn = true;
    }
}

IMPLEMENT_DIALOG(gui_smith_chart);