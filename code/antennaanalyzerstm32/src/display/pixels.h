#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void put_pixel(int x, int y, uint16_t color);
void put_smith_pixel(int x, int y, uint16_t color, int mid_x, int mid_y, int max_radius);
void draw_circle(int x0, int y0, int radius, uint16_t color);
void draw_limited_circle(int x0, int y0, int radius, uint16_t color, int mid_x, int mid_y, int max_radius);
void plot_line_low(int x0, int y0, int x1, int y1, uint16_t color);
void plot_line_high(int x0, int y0, int x1, int y1, uint16_t color);
void draw_line(int x0, int y0, int x1, int y1, uint16_t color);

#ifdef __cplusplus
}
#endif
