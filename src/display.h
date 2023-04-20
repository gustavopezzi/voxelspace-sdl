#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

#define FPS 30
#define MILLISECS_PER_FRAME (1000 / FPS)

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

bool init_window(void);
void destroy_window(void);

void framerate_fix(void);

void clear_framebuffer(uint32_t color);
void render_framebuffer(void);

void draw_pixel(uint16_t x, uint16_t y, uint32_t color);
void draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color);
void draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

#endif