#include "pixels.h"
#include "ILI9341_STM32_Driver.h"
#include "stm32f4xx_hal.h"
#include "arm_math.h"
#include <stdlib.h>

#include "serial.h"

void put_pixel(int x, int y, uint16_t color)
{
	ILI9341_Draw_Pixel(x, y, color);
    //HAL_Delay(1);
}

void put_smith_pixel(int x, int y, uint16_t color, int mid_x, int mid_y, int max_radius)
{
	int x_rel = x - mid_x;
	int y_rel = y - mid_y;

	int dist = x_rel * x_rel + y_rel * y_rel;

	if (dist < (max_radius*max_radius))
	{
		put_pixel(x, y, color);
	}
}

void draw_limited_circle(int x0, int y0, int radius, uint16_t color, int mid_x, int mid_y, int max_radius)
{
    int x = radius - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (radius << 1);

	while (x >= y)
	{
		put_smith_pixel(x0 + x, y0 + y, color, mid_x, mid_y, max_radius);
		put_smith_pixel(x0 + y, y0 + x, color, mid_x, mid_y, max_radius);
		put_smith_pixel(x0 - y, y0 + x, color, mid_x, mid_y, max_radius);
		put_smith_pixel(x0 - x, y0 + y, color, mid_x, mid_y, max_radius);
		put_smith_pixel(x0 - x, y0 - y, color, mid_x, mid_y, max_radius);
		put_smith_pixel(x0 - y, y0 - x, color, mid_x, mid_y, max_radius);
		put_smith_pixel(x0 + y, y0 - x, color, mid_x, mid_y, max_radius);
		put_smith_pixel(x0 + x, y0 - y, color, mid_x, mid_y, max_radius);

		if (err <= 0)
		{
			y++;
			err += dy;
			dy += 2;
		}

		if (err > 0)
		{
			x--;
			dx += 2;
			err += dx - (radius << 1);
		}
	}
}

void draw_circle(int x0, int y0, int radius, uint16_t color)
{
	int x = radius - 1;
	int y = 0;
	int dx = 1;
	int dy = 1;
	int err = dx - (radius << 1);

	while (x >= y)
	{
		put_pixel(x0 + x, y0 + y, color);
		put_pixel(x0 + y, y0 + x, color);
		put_pixel(x0 - y, y0 + x, color);
		put_pixel(x0 - x, y0 + y, color);
		put_pixel(x0 - x, y0 - y, color);
		put_pixel(x0 - y, y0 - x, color);
		put_pixel(x0 + y, y0 - x, color);
		put_pixel(x0 + x, y0 - y, color);

		if (err <= 0)
		{
			y++;
			err += dy;
			dy += 2;
		}

		if (err > 0)
		{
			x--;
			dx += 2;
			err += dx - (radius << 1);
		}
	}
}

void plot_line_low(int x0, int y0, int x1, int y1, uint16_t color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int yi = 1;
	if (dy < 0)
	{
		yi = -1;
		dy = -dy;
	}

	int D = 2 * dy - dx;
	int y = y0;

	for (int x = x0; x <= x1; x++)
	{
		put_pixel(x, y, color);
		if (D > 0)
		{
			y = y + yi;
			D = D - 2 * dx;
		}
		D = D + 2 * dy;
	}
}

void plot_line_high(int x0, int y0, int x1, int y1, uint16_t color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int xi = 1;
	if (dx < 0)
	{
		xi = -1;
		dx = -dx;
	}
	int D = 2 * dx - dy;
	int x = x0;

	for (int y = y0; y <= y1; y++)
	{
		put_pixel(x, y, color);
		if (D > 0)
		{
			x = x + xi;
			D = D - 2 * dy;
		}
		D = D + 2 * dx;
	}
}

void draw_line(int x0, int y0, int x1, int y1, uint16_t color)
{
	if (abs(y1 - y0) < abs(x1 - x0))
	{
		if (x0 > x1)
		{
			plot_line_low(x1, y1, x0, y0, color);
		}
		else
		{
			plot_line_low(x0, y0, x1, y1, color);
		}
	}
	else
	{
		if (y0 > y1)
		{
			plot_line_high(x1, y1, x0, y0, color);
		}
		else
		{
			plot_line_high(x0, y0, x1, y1, color);
		}
	}
}
