#include "display.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static uint32_t *framebuffer = NULL;

static SDL_Texture *framebuffer_texture = NULL;

static uint16_t window_width = 800;
static uint16_t window_height = 800;

bool init_window(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }

  window = SDL_CreateWindow("voxelspace", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, 0);
  if (!window) {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    fprintf(stderr, "Error creating SDL renderer.\n");
    return false;
  }

  framebuffer = malloc(sizeof(uint32_t) * SCREEN_WIDTH * SCREEN_HEIGHT);
  
  framebuffer_texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_RGBA32,
    SDL_TEXTUREACCESS_STREAMING,
    SCREEN_WIDTH,
    SCREEN_HEIGHT
  );

  return true;
}

void framerate_fix(void) {
  static int32_t previous_frame_time = 0;
  int time_ellapsed = SDL_GetTicks() - previous_frame_time;
  int time_to_wait = MILLISECS_PER_FRAME - time_ellapsed;
  if (time_to_wait > 0 && time_to_wait < MILLISECS_PER_FRAME) {
    SDL_Delay(time_to_wait);
  }
  previous_frame_time = SDL_GetTicks();
}

void draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
    return;
  }
  framebuffer[(SCREEN_WIDTH * y) + x] = color;
}

void draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  float err = ((dx > dy) ? dx : -dy) / 2.0;
  while (true) {
    draw_pixel(x0, y0, color);
    if (x0 == x1 && y0 == y1) {
      break;
    }
    float e2 = err;
    if (e2 > -dx) {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dy) {
      err += dx;
      y0 += sy;
    }
  }
}

void draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (size_t i = 0; i < width; i++) {
    for (size_t j = 0; j < height; j++) {
      uint16_t current_x = x + i;
      uint16_t current_y = y + j;
      draw_pixel(current_x, current_y, color);
    }
  }
}

void render_framebuffer(void) {
  SDL_UpdateTexture(
    framebuffer_texture,
    NULL,
    framebuffer,
    (int)(SCREEN_WIDTH * sizeof(uint32_t))
  );
  SDL_RenderCopy(renderer, framebuffer_texture, NULL, NULL);
  SDL_RenderPresent(renderer);
}

void clear_framebuffer(uint32_t color) {
  for (size_t i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    framebuffer[i] = color;
  }
}

void destroy_window(void) {
  free(framebuffer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}