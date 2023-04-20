#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "display.h"
#include "input.h"
#include "player.h"
#include "../libs/gif/gifload.h"

#define NIGHT_MODE

#define MAP_N 1024
#define Z_FAR 1000.0
#define VERTICAL_SCALE_FACTOR 35.0

bool is_running = false;

uint32_t palette[256*3];

uint8_t *heightmap = NULL;
uint8_t *colormap  = NULL;

player_t player = {
  .x           = 512.0,
  .y           = 512.0,
  .height      = 130.0,
  .angle       =   0.0,
  .pitch       =   0.0,
  .forward_vel =   0.0,   .forward_acc = 0.06,   .forward_brk = 0.06,   .forward_max = 1.0,
  .pitch_vel   =   0.0,   .pitch_acc   = 0.06,   .pitch_brk   = 0.10,   .pitch_max   = 1.0,
  .yaw_vel     =   0.0,   .yaw_acc     = 0.10,   .yaw_brk     = 0.10,   .yaw_max     = 1.0,
  .lift_vel    =   0.0,   .lift_acc    = 0.06,   .lift_brk    = 0.07,   .lift_max    = 1.0,
  .strafe_vel  =   0.0,   .strafe_acc  = 0.05,   .strafe_brk  = 0.09,   .strafe_max  = 1.0,
  .roll_vel    =   0.0,   .roll_acc    = 0.04,   .roll_brk    = 0.09,   .roll_max    = 1.0
};

void load_map(void) {
  uint8_t gif_palette[256 * 3];
  int pal_count;

  colormap = load_gif("maps/gif/map28.color.gif", &pal_count, gif_palette);
  heightmap = load_gif("maps/gif/map28.height.gif", NULL, NULL);

  for (int i = 0; i < pal_count; i++) {
    int r = gif_palette[3 * i + 0];
    int g = gif_palette[3 * i + 1];
    int b = gif_palette[3 * i + 2];
    r = (r & 63) << 2;
    g = (g & 63) << 2;
    b = (b & 63) << 2;
    #ifdef NIGHT_MODE
      palette[i] = (0 << 16) | (g << 8) | 0;
    #else
      palette[i] = (b << 16) | (g << 8) | r;
    #endif
  }
}

void process_input(void) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        is_running = false;
        break;
      case SDL_KEYDOWN:
        key_down(event.key.keysym.sym);
        break;
      case SDL_KEYUP:
        key_up(event.key.keysym.sym);
        break;
    }
  }
}

void update(void) {
  player_move(&player);
}

void draw(void) {
  #ifdef NIGHT_MODE
    clear_framebuffer(0xFF0D2900);
  #else
    clear_framebuffer(0xFFE0BB36);
  #endif

  // Pre-compute sine and cosine of the rotation angle
  float sin_angle = sin(player.angle);
  float cos_angle = cos(player.angle);

  // Left-most point of the FOV
  float pl_x = cos_angle * Z_FAR + sin_angle * Z_FAR;
  float pl_y = sin_angle * Z_FAR - cos_angle * Z_FAR;

  // Right-most point of the FOV
  float pr_x = cos_angle * Z_FAR - sin_angle * Z_FAR;
  float pr_y = sin_angle * Z_FAR + cos_angle * Z_FAR;

  // Loop 320 rays from left to right
  for (int i = 0; i < SCREEN_WIDTH; i++) {
    float delta_x = (pl_x + (pr_x - pl_x) / SCREEN_WIDTH * i) / Z_FAR;
    float delta_y = (pl_y + (pr_y - pl_y) / SCREEN_WIDTH * i) / Z_FAR;

    // Ray (x,y) coords
    float r_x = player.x;
    float r_y = player.y;

    // Store the tallest projected height per-ray
    float tallest_height = SCREEN_HEIGHT;

    // Loop all depth units until the zfar distance limit
    for (int z = 1; z < Z_FAR; z++) {
      r_x += delta_x;
      r_y += delta_y;

      // Find the offset that we have to go and fetch values from the heightmap
      int map_offset = ((MAP_N * ((int)(r_y) & (MAP_N - 1))) + ((int)(r_x) & (MAP_N - 1)));

      // Project height values and find the height on-screen
      int proj_height = (int)((player.height - heightmap[map_offset]) / z * VERTICAL_SCALE_FACTOR + player.pitch);

      // Only draw pixels if the new projected height is taller than the previous tallest height
      if (proj_height < tallest_height) {
        float tilt = (player.roll_vel * (i / (float)SCREEN_WIDTH - 0.5) + 0.5) * SCREEN_HEIGHT / 6.0;

        // Draw pixels from previous max-height until the new projected height
        for (int y = (proj_height + tilt); y < (tallest_height + tilt); y++) {
          if (y >= 0) {
            draw_pixel(i, y, palette[colormap[map_offset]]);
          }
        }
        tallest_height = proj_height;
      }
    }
  }

  // Draw HUD lines on top of the terrain
  int mid_width = SCREEN_WIDTH / 2;
  int mid_height = SCREEN_HEIGHT / 2;
  draw_pixel(mid_width, mid_height, 0xFF00FF00);
  draw_line(mid_width,      mid_height - 10, mid_width,      mid_height - 40, 0xFF00FF00);
  draw_line(mid_width,      mid_height + 10, mid_width,      mid_height + 40, 0xFF00FF00);
  draw_line(mid_width - 10, mid_height,      mid_width - 40, mid_height,      0xFF00FF00);
  draw_line(mid_width + 10, mid_height,      mid_width + 40, mid_height,      0xFF00FF00);

  // Render framebuffer to SDL texture to be displayed
  render_framebuffer();
}

int main(int argc, char* args[]) {
  is_running = init_window();

  load_map();

  while (is_running) {
    framerate_fix();
    process_input();
    update();
    draw();
  }

  destroy_window();

  return EXIT_SUCCESS;
}