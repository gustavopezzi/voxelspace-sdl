#include "player.h"
#include "input.h"
#include <math.h>
#include <SDL.h>

void player_move(player_t* player) {
  // Move front or back (accounting for acceleration & deacceleration)
  if (is_key_pressed(SDLK_w) && player->forward_vel < player->forward_max) {
    player->forward_vel += (player->forward_vel < 0) ? player->forward_brk : player->forward_acc;
  } else if (is_key_pressed(SDLK_s) && player->forward_vel > -player->forward_max) {
    player->forward_vel -= (player->forward_vel > 0) ? player->forward_brk : player->forward_acc; 
  } else {
    if (player->forward_vel - player->forward_brk > 0) {
      player->forward_vel -= player->forward_brk;
    } else if (player->forward_vel + player->forward_brk < 0) {
      player->forward_vel += player->forward_brk;
    } else {
      player->forward_vel = 0;
    }
  }

  // Pitch down and up as we move forward and backwards (accounting for acceleration & deacceleration)
  if (is_key_pressed(SDLK_w) && player->pitch_vel > -player->pitch_max) {
    player->pitch_vel -= (player->pitch_vel > 0) ? player->pitch_brk : player->pitch_acc;
  } else if (is_key_pressed(SDLK_s) && player->pitch_vel < player->pitch_max - player->pitch_acc) {
    player->pitch_vel += (player->pitch_vel < 0) ? player->pitch_brk : player->pitch_acc;
  } else if (!is_key_pressed(SDLK_w) && !is_key_pressed(SDLK_s)) {
    if (player->pitch_vel - player->pitch_brk > 0) {
      player->pitch_vel -= player->pitch_brk;
    } else if (player->pitch_vel + player->pitch_brk < 0) {
      player->pitch_vel += player->pitch_brk;
    } else {
      player->pitch_vel = 0;
    }
  }

  // Yaw left or right (accounting for acceleration & deacceleration)
  if (is_key_pressed(SDLK_a) && player->yaw_vel > -player->yaw_max) {
    player->yaw_vel -= (player->yaw_vel > 0) ? player->yaw_brk : player->yaw_acc;
  } else if (is_key_pressed(SDLK_d) && player->yaw_vel < player->yaw_max) {
    player->yaw_vel += (player->yaw_vel < 0) ? player->yaw_brk : player->yaw_acc;
  } else {
    if (player->yaw_vel - player->yaw_brk > 0) {
      player->yaw_vel -= player->yaw_brk;
    } else if (player->yaw_vel + player->yaw_brk < 0) {
      player->yaw_vel += player->yaw_brk;
    } else {
      player->yaw_vel = 0.0f;
    }
  }

  // Roll left or right (accounting for acceleration & deacceleration)
  if (is_key_pressed(SDLK_d) && player->roll_vel > -player->roll_max) {
    player->roll_vel -= (player->roll_vel > 0) ? player->roll_brk : player->roll_acc;
  } else if (is_key_pressed(SDLK_a) && player->roll_vel < player->roll_max - player->roll_acc) {
    player->roll_vel += (player->roll_vel < 0) ? player->roll_brk : player->roll_acc;
  } else if (!is_key_pressed(SDLK_a) && !is_key_pressed(SDLK_d) && !is_key_pressed(SDLK_m) && !is_key_pressed(SDLK_n)) {
    if (player->roll_vel - player->roll_brk > 0) {
      player->roll_vel -= player->roll_brk;
    } else if (player->roll_vel + player->roll_brk < 0) {
      player->roll_vel += player->roll_brk;
    } else {
      player->roll_vel = 0.0f;
    }
  }

  // Move up and down (accounting for acceleration & deacceleration)
  if (is_key_pressed(SDLK_u) && player->lift_vel < player->lift_max) {
    player->lift_vel += (player->lift_vel < 0) ? player->lift_brk : player->lift_acc;
  } else if (is_key_pressed(SDLK_j) && player->lift_vel > -player->lift_max) {
    player->lift_vel -= (player->lift_vel > 0) ? player->lift_brk : player->lift_acc;
  } else {
    if (player->lift_vel - player->lift_brk > 0) {
      player->lift_vel -= player->lift_brk;
    } else if (player->lift_vel + player->lift_brk < 0) {
      player->lift_vel += player->lift_brk;
    } else {
      player->lift_vel = 0.0f;
    }
  }

  // Strafe left and right (accounting for acceleration & deacceleration)
  if (is_key_pressed(SDLK_m) && player->strafe_vel < player->strafe_max) {
    player->strafe_vel += (player->strafe_vel < 0) ? player->strafe_brk : player->strafe_acc;
  } else if (is_key_pressed(SDLK_n) && player->strafe_vel > -player->strafe_max) {
    player->strafe_vel -= (player->strafe_vel > 0) ? player->strafe_brk : player->strafe_acc;
  } else {
    if (player->strafe_vel - player->strafe_brk > 0) {
      player->strafe_vel -= player->strafe_brk;
    } else if (player->strafe_vel + player->strafe_brk < 0) {
      player->strafe_vel += player->strafe_brk;
    } else {
      player->strafe_vel = 0.0f;
    }
  }

  // Roll left or right as we strafe (accounting for acceleration & deacceleration)
  if (is_key_pressed(SDLK_m) && player->roll_vel > -player->roll_max) {
    player->roll_vel -= (player->roll_vel > 0) ? player->roll_brk : player->roll_acc;
  } else if (is_key_pressed(SDLK_n) && player->roll_vel < player->roll_max - player->roll_acc) {
    player->roll_vel += (player->roll_vel < 0) ? player->roll_brk : player->roll_acc;
  } else if (!is_key_pressed(SDLK_a) && !is_key_pressed(SDLK_d) && !is_key_pressed(SDLK_m) && !is_key_pressed(SDLK_n)) {
    if (player->roll_vel - player->roll_brk > 0) {
      player->roll_vel -= player->roll_brk;
    } else if (player->roll_vel + player->roll_brk < 0) {
      player->roll_vel += player->roll_brk;
    } else {
      player->roll_vel = 0.0f;
    }
  }
  
  // Update player x and y position based on forward velocity and angle
  player->x += cos(player->angle) * player->forward_vel * 0.9;
  player->y += sin(player->angle) * player->forward_vel * 0.9;

  // Strafe left and right perpendicular to the player angle
  player->x += cos(1.57 + player->angle) * player->strafe_vel * 0.5;
  player->y += sin(1.57 + player->angle) * player->strafe_vel * 0.5;

  // Update player angle based on its current yaw velocity
  player->angle += player->yaw_vel * 0.02;

  // Update player height based on lift velocity
  player->height += player->lift_vel * 1.4;

  // Pitch up and down as we move front and back
  player->pitch = (player->pitch_vel * 20.0) + 80.0;
}