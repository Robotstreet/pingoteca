#include "ball.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

void ball_init(Ball* ball, float x, float y) {
    ball->x = x;
    ball->y = y;
    ball->vx = 300.0f;
    ball->vy = 0.0f;
    ball->radius = 8.0f;
    ball->speed = 300.0f;
    ball->max_speed = 800.0f;
    ball->active = true;
    
    if (rand() % 2) ball->vx = -ball->vx;
    if (rand() % 2) ball->vy = -ball->vy;

    ball->is_clone = false;
    ball->is_real_ball = true;
    ball->invisible = false;
    ball->phasing = false;
    ball->homing = false;

    ball->zigzag_mode = false;
    ball->zigzag_amplitude = 0.0f;
    ball->zigzag_frequency = 0.0f;
    ball->zigzag_timer = 0.0f;

    ball->curve_mode = false;
    ball->curve_strength = 0.0f;

    ball->spiral_mode = false;
    ball->spiral_radius = 0.0f;
    ball->spiral_angle = 0.0f;

    ball->gravity_affected = false;
    ball->gravity_multiplier = 1.0f;

    ball->teleport_mode = false;
    ball->teleport_timer = 0.0f;
    ball->teleport_interval = 2.0f;

    ball->heavy = false;
    ball->light = false;
    ball->magnetic = false;
    ball->electric = false;
    ball->ice = false;
    ball->poison = false;
    ball->explosive = false;

    ball->effect_timer = 0.0f;
    ball->bounce_multiplier = 1;
    
    ball->gravity_scale = 1.0f;
    ball->affected_by_wind = true;
}

void ball_reset(Ball* ball, float x, float y) {
    ball_init(ball, x, y);
}

void ball_update(Ball* ball, float delta_time) {
    if (!ball->active) return;

    ball_apply_special_physics(ball, delta_time);

    if (ball->fire_timer > 0.0f) {
        ball->fire_timer -= delta_time;
        if (ball->fire_timer <= 0.0f) {
            ball->fire_mode = false;
        }
    }

    if (ball->effect_timer > 0.0f) {
        ball->effect_timer -= delta_time;
        if (ball->effect_timer <= 0.0f) {
            ball->zigzag_mode = false;
            ball->curve_mode = false;
            ball->spiral_mode = false;
            ball->homing = false;
            ball->invisible = false;
            ball->phasing = false;
            ball->teleport_mode = false;
        }
    }

    ball->x += ball->vx * delta_time;
    ball->y += ball->vy * delta_time;

    ball->trail_count++;
    if (ball->trail_count > 60) ball->trail_count = 0;
}



void ball_apply_special_physics(Ball* ball, float delta_time) {
    if (ball->zigzag_mode) {
        ball->zigzag_timer += delta_time;
        float zigzag_offset = sin(ball->zigzag_timer * ball->zigzag_frequency) * ball->zigzag_amplitude;

        float perpendicular_x = -ball->vy / ball->speed;
        float perpendicular_y = ball->vx / ball->speed;

        ball->x += perpendicular_x * zigzag_offset * delta_time;
        ball->y += perpendicular_y * zigzag_offset * delta_time;
    }

    if (ball->curve_mode) {
        float curve_force = ball->curve_strength * delta_time;
        ball->vy += curve_force;

        float current_speed = sqrt(ball->vx * ball->vx + ball->vy * ball->vy);
        if (current_speed > 0) {
            ball->vx = (ball->vx / current_speed) * ball->speed;
            ball->vy = (ball->vy / current_speed) * ball->speed;
        }
    }

    if (ball->spiral_mode) {
        ball->spiral_angle += delta_time * 5.0f; // Velocidade angular

        float spiral_x = cos(ball->spiral_angle) * ball->spiral_radius;
        float spiral_y = sin(ball->spiral_angle) * ball->spiral_radius;

        ball->x += spiral_x * delta_time;
        ball->y += spiral_y * delta_time;
    }

    if (ball->teleport_mode) {
        ball->teleport_timer -= delta_time;
        if (ball->teleport_timer <= 0.0f) {
            ball->x = 100 + (rand() % 600); // Assumindo tela 800x600
            ball->y = 100 + (rand() % 400);
            ball->teleport_timer = ball->teleport_interval;
        }
    }

    if (ball->homing) {
    }

    if (ball->gravity_affected) {
        ball->vy += 98.0f * ball->gravity_multiplier * delta_time; // Gravidade personalizada
    }
}

void ball_clamp_speed(Ball* ball) {
    float current_speed = sqrt(ball->vx * ball->vx + ball->vy * ball->vy);
    
    if (current_speed > ball->max_speed) {
        float ratio = ball->max_speed / current_speed;
        ball->vx *= ratio;
        ball->vy *= ratio;
        ball->speed = ball->max_speed;
    } else {
        ball->speed = current_speed;
    }
}

void ball_activate_zigzag(Ball* ball, float amplitude, float frequency, float duration) {
    ball->zigzag_mode = true;
    ball->zigzag_amplitude = amplitude;
    ball->zigzag_frequency = frequency;
    ball->zigzag_timer = 0.0f;
    ball->effect_timer = duration;
}

void ball_activate_curve(Ball* ball, float strength, float duration) {
    ball->curve_mode = true;
    ball->curve_strength = strength;
    ball->effect_timer = duration;
}

void ball_activate_spiral(Ball* ball, float radius, float duration) {
    ball->spiral_mode = true;
    ball->spiral_radius = radius;
    ball->spiral_angle = 0.0f;
    ball->effect_timer = duration;
}

float ball_get_effective_radius(Ball* ball) {
    return ball->radius;
}


void ball_activate_homing(Ball* ball, float duration) {
    ball->homing = true;
    ball->effect_timer = duration;
}

void ball_activate_teleport(Ball* ball, float interval, float duration) {
    ball->teleport_mode = true;
    ball->teleport_interval = interval;
    ball->teleport_timer = interval;
    ball->effect_timer = duration;
}

void ball_make_invisible(Ball* ball, float duration) {
    ball->invisible = true;
    ball->effect_timer = duration;
}

void ball_enable_phasing(Ball* ball, float duration) {
    ball->phasing = true;
    ball->effect_timer = duration;
}

Ball* ball_create_clone(Ball* original, bool is_real) {
    Ball* clone = malloc(sizeof(Ball));
    memcpy(clone, original, sizeof(Ball));

    clone->is_clone = true;
    clone->is_real_ball = is_real;

    clone->x += (rand() % 20) - 10;
    clone->y += (rand() % 20) - 10;

    return clone;
}

void ball_apply_velocity(Ball* ball, float vx, float vy) {
    ball->vx = vx;
    ball->vy = vy;

    ball->speed = sqrt(vx * vx + vy * vy);
    if (ball->speed > ball->max_speed) {
        ball->vx = (vx / ball->speed) * ball->max_speed;
        ball->vy = (vy / ball->speed) * ball->max_speed;
        ball->speed = ball->max_speed;
    }
}

void ball_apply_spin(Ball* ball, float spin_amount) {
    ball->spin = spin_amount;

    if (ball->spin != 0.0f) {
        ball->curve_mode = true;
        ball->curve_strength = ball->spin * 50.0f;
    }
}

void ball_increase_speed(Ball* ball, float multiplier) {
    ball->speed *= multiplier;
    if (ball->speed > ball->max_speed) {
        ball->speed = ball->max_speed;
    }

    float current_speed = sqrt(ball->vx * ball->vx + ball->vy * ball->vy);
    if (current_speed > 0) {
        ball->vx = (ball->vx / current_speed) * ball->speed;
        ball->vy = (ball->vy / current_speed) * ball->speed;
    }
}

void ball_activate_fire_mode(Ball* ball, float duration) {
    ball->fire_mode = true;
    ball->fire_timer = duration;
}

bool ball_is_fire_mode_active(Ball* ball) {
    return ball->fire_mode && ball->fire_timer > 0.0f;
}

void ball_reverse_x(Ball* ball) {
    ball->vx = -ball->vx;
    ball->bounce_count++;
}

void ball_reverse_y(Ball* ball) {
    ball->vy = -ball->vy;
    ball->bounce_count++;
}

float ball_get_speed(Ball* ball) {
    return ball->speed;
}

void ball_set_position(Ball* ball, float x, float y) {
    ball->x = x;
    ball->y = y;
}

void ball_add_trail_effect(Ball* ball) {
    ball->trail_count++;
}
