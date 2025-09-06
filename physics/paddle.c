#include "paddle.h"
#include <math.h>
#include <stdlib.h>

#define SCREEN_HEIGHT 600

void paddle_init(Paddle* paddle, float x, float y, bool is_player, int side) {
    paddle->x = x;
    paddle->y = y;
    paddle->width = 20.0f;
    paddle->height = 100.0f;
    paddle->speed = 400.0f;
    paddle->score = 0;
    paddle->side = side;
}

void paddle_update(Paddle* paddle, float delta_time) {
    paddle_check_bounds(paddle, SCREEN_HEIGHT);
}

void paddle_move_up(Paddle* paddle, float delta_time) {
    paddle->y -= paddle->speed * delta_time;
}

void paddle_move_down(Paddle* paddle, float delta_time) {
    paddle->y += paddle->speed * delta_time;
}

void paddle_set_target(Paddle* paddle, float target_y) {
    float diff = target_y - paddle->y;
    if (fabs(diff) > 5.0f) {
        if (diff > 0) {
            paddle->y += paddle->speed * 0.016f; // ~60 FPS
        } else {
            paddle->y -= paddle->speed * 0.016f;
        }
    }
}

void paddle_move_to_target(Paddle* paddle, float delta_time) {
}

bool paddle_check_bounds(Paddle* paddle, float screen_height) {
    if (paddle->y < 0) {
        paddle->y = 0;
        return false;
    }
    
    if (paddle->y + paddle->height > screen_height) {
        paddle->y = screen_height - paddle->height;
        return false;
    }
    
    return true;
}

HitZone paddle_get_hit_zone(Paddle* paddle, float ball_y) {
    float relative_y = ball_y - paddle->y;
    float zone_height = paddle->height / 4.0f;
    
    if (relative_y < zone_height) {
        return HIT_ZONE_TOP;
    } else if (relative_y < zone_height * 3) {
        return HIT_ZONE_CENTER;
    } else if (relative_y < paddle->height) {
        return HIT_ZONE_BOTTOM;
    } else {
        return HIT_ZONE_EDGE;
    }
}

float paddle_get_effective_height(Paddle* paddle) {
    return paddle->height;
}

float paddle_get_effective_speed(Paddle* paddle) {
    return paddle->speed;
}

bool paddle_has_shield(Paddle* paddle) {
    return false; // Sempre falso na versão simples
}

void paddle_register_hit(Paddle* paddle, bool perfect) {
}

void paddle_reset_powerups(Paddle* paddle) {
}

void paddle_activate_shield(Paddle* paddle, float duration) {}
void paddle_activate_size_boost(Paddle* paddle, float duration) {}
void paddle_activate_speed_boost(Paddle* paddle, float duration) {}
void paddle_apply_slow(Paddle* paddle, float multiplier, float duration) {}
void paddle_apply_blind(Paddle* paddle, float duration) {}
void paddle_apply_confusion(Paddle* paddle, float duration) {}
void paddle_apply_freeze(Paddle* paddle, float duration) {}
void paddle_apply_shrink(Paddle* paddle, float multiplier, float duration) {}
void paddle_apply_dizzy(Paddle* paddle, float duration) {}
void paddle_update_debuffs(Paddle* paddle, float delta_time) {}
bool paddle_is_debuffed(Paddle* paddle) { return false; }
