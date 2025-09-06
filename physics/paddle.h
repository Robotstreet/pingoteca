#ifndef PADDLE_H
#define PADDLE_H

#include <stdbool.h>

typedef struct {
    float x, y;
    float width, height;
    float speed;
    int score;
    int side; // 0 = esquerda, 1 = direita
} Paddle;

typedef enum {
    HIT_ZONE_TOP = 0,
    HIT_ZONE_CENTER,
    HIT_ZONE_BOTTOM,
    HIT_ZONE_EDGE
} HitZone;

void paddle_init(Paddle* paddle, float x, float y, bool is_player, int side);
void paddle_update(Paddle* paddle, float delta_time);
void paddle_move_up(Paddle* paddle, float delta_time);
void paddle_move_down(Paddle* paddle, float delta_time);
void paddle_set_target(Paddle* paddle, float target_y);
void paddle_move_to_target(Paddle* paddle, float delta_time);
bool paddle_check_bounds(Paddle* paddle, float screen_height);
HitZone paddle_get_hit_zone(Paddle* paddle, float ball_y);

float paddle_get_effective_height(Paddle* paddle);
float paddle_get_effective_speed(Paddle* paddle);
bool paddle_has_shield(Paddle* paddle);
void paddle_register_hit(Paddle* paddle, bool perfect);
void paddle_reset_powerups(Paddle* paddle);

void paddle_activate_shield(Paddle* paddle, float duration);
void paddle_activate_size_boost(Paddle* paddle, float duration);
void paddle_activate_speed_boost(Paddle* paddle, float duration);
void paddle_apply_slow(Paddle* paddle, float multiplier, float duration);
void paddle_apply_blind(Paddle* paddle, float duration);
void paddle_apply_confusion(Paddle* paddle, float duration);
void paddle_apply_freeze(Paddle* paddle, float duration);
void paddle_apply_shrink(Paddle* paddle, float multiplier, float duration);
void paddle_apply_dizzy(Paddle* paddle, float duration);
void paddle_update_debuffs(Paddle* paddle, float delta_time);
bool paddle_is_debuffed(Paddle* paddle);

#endif // PADDLE_H
