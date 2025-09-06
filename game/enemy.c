#include "enemy.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>



static const struct {
    const char* name;
    float reaction_time;
    float prediction_accuracy;
    float move_speed;
    float aggression;
    float defense_bias;
    float error_rate;
    int health;
} enemy_configs[8] = {
    {"Iniciante", 0.8f, 0.3f, 200.0f, 0.2f, 0.8f, 0.3f, 100},
    {"Defensor", 0.6f, 0.5f, 150.0f, 0.1f, 0.9f, 0.2f, 150},
    {"Agressor", 0.4f, 0.7f, 300.0f, 0.9f, 0.1f, 0.15f, 120},
    {"Adaptativo", 0.5f, 0.6f, 250.0f, 0.5f, 0.5f, 0.1f, 200},
    {"Boss Tempestade", 0.3f, 0.8f, 180.0f, 0.7f, 0.6f, 0.05f, 500},
    {"Boss Relâmpago", 0.2f, 0.9f, 400.0f, 0.8f, 0.3f, 0.03f, 750},
    {"Boss Caos", 0.4f, 0.7f, 320.0f, 0.9f, 0.2f, 0.08f, 600},
    {"Boss Mestre", 0.1f, 0.95f, 280.0f, 0.6f, 0.7f, 0.01f, 1000}
};

void enemy_update_powers(Enemy* enemy, Ball* ball, Paddle* player_paddle, float delta_time);
void enemy_activate_power(Enemy* enemy, Ball* ball, Paddle* player_paddle);
void enemy_update_active_power(Enemy* enemy, Ball* ball, Paddle* player_paddle, float delta_time);
void enemy_update_visual_effects(Enemy* enemy, float delta_time);

void enemy_activate_erro_bobo(Enemy* enemy, Ball* ball);
void enemy_activate_muralha_aco(Enemy* enemy);
void enemy_activate_bolada_atordoante(Enemy* enemy, Ball* ball, Paddle* player_paddle);
void enemy_activate_troca_percurso(Enemy* enemy, Ball* ball);
void enemy_activate_neblina_densa(Enemy* enemy);
void enemy_activate_choque_paralisante(Enemy* enemy, Paddle* player_paddle);
void enemy_activate_bola_caotica(Enemy* enemy, Ball* ball);
void enemy_activate_clone_sombras(Enemy* enemy);
void enemy_activate_dominio_absoluto(Enemy* enemy, Ball* ball, Paddle* player_paddle);

void enemy_update_muralha_aco(Enemy* enemy, Ball* ball, float delta_time);
void enemy_update_neblina_densa(Enemy* enemy, float delta_time);
void enemy_update_bola_caotica(Enemy* enemy, Ball* ball, float delta_time);
void enemy_update_clone_sombras(Enemy* enemy, float delta_time);
void enemy_update_dominio_absoluto(Enemy* enemy, Ball* ball, Paddle* player_paddle, float delta_time);

static void enemy_start_pre_power_effect(Enemy* enemy, Ball* ball, Paddle* player_paddle);
static void enemy_update_electric_ray(Enemy* enemy, Paddle* player_paddle, float delta_time);


void enemy_init(Enemy* enemy, EnemyType type, int level) {
    if (!enemy || type < 0 || type >= 8) return;

    memset(enemy, 0, sizeof(Enemy));

    enemy->type = type;
    enemy->level = level;
    strcpy(enemy->name, enemy_configs[type].name);

    enemy->health = enemy_configs[type].health + (level * 50);
    enemy->max_health = enemy->health;
    enemy->difficulty_multiplier = 1.0f + (level * 0.2f);

    enemy->ai_state = AI_STATE_TRACKING;
    enemy->ai_timer = 0.0f;
    enemy->reaction_time = enemy_configs[type].reaction_time * (1.0f - level * 0.05f);
    enemy->prediction_accuracy = enemy_configs[type].prediction_accuracy + (level * 0.05f);
    enemy->aggression = enemy_configs[type].aggression;
    enemy->defense_bias = enemy_configs[type].defense_bias;
    enemy->error_rate = enemy_configs[type].error_rate * (1.0f - level * 0.02f);

    enemy->move_speed_base = enemy_configs[type].move_speed;
    enemy->move_speed_current = enemy->move_speed_base;
    enemy->target_y = 300.0f; // Centro da tela
    enemy->center_bias = 0.3f;
    enemy->edge_avoidance = 0.8f;

    enemy->can_use_powerups = (type >= ENEMY_ADAPTIVE);
    enemy->powerup_cooldown = 0.0f;
    enemy->special_move_cooldown = 0.0f;
    enemy->in_special_move = false;
    enemy->special_duration = 2.0f;

    enemy->movement_pattern = PATTERN_STRAIGHT;
    enemy->pattern_timer = 0.0f;
    enemy->pattern_amplitude = 50.0f;
    enemy->pattern_frequency = 1.0f;

    enemy->is_stunned = false;
    enemy->stun_duration = 0.0f;
    enemy->is_enraged = false;
    enemy->rage_multiplier = 1.0f;


    enemy->hits_made = 0;
    enemy->hits_received = 0;
    enemy->special_moves_used = 0;
    enemy->damage_dealt = 0.0f;
    enemy->accuracy = 0.0f;
    
    
    switch (type) {
        case ENEMY_BEGINNER:
            enemy->power_type = POWER_ERRO_BOBO;
            enemy->power_cooldown = 3.0f;
            enemy->power_duration = 0.5f;
            break;
        case ENEMY_DEFENDER:
            enemy->power_type = POWER_MURALHA_ACO;
            enemy->power_cooldown = 8.0f;
            enemy->power_duration = 3.0f;
            break;
        case ENEMY_AGGRESSOR:
            enemy->power_type = POWER_BOLADA_ATORDOANTE;
            enemy->power_cooldown = 6.0f;
            enemy->power_duration = 1.0f;
            break;
        case ENEMY_ADAPTIVE:
            enemy->power_type = POWER_TROCA_PERCURSO;
            enemy->power_cooldown = 5.0f;
            enemy->power_duration = 0.3f;
            break;
        case ENEMY_BOSS_STORM:
            enemy->power_type = POWER_NEBLINA_DENSA;
            enemy->power_cooldown = 10.0f;
            enemy->power_duration = 4.0f;
            break;
        case ENEMY_BOSS_LIGHTNING:
            enemy->power_type = POWER_CHOQUE_PARALISANTE;
            enemy->power_cooldown = 7.0f;
            enemy->power_duration = 1.0f;
            break;
        case ENEMY_BOSS_CHAOS:
            enemy->power_type = POWER_BOLA_CAOTICA;
            enemy->power_cooldown = 12.0f;
            enemy->power_duration = 5.0f;
            break;
        case ENEMY_BOSS_MASTER:
            enemy->power_type = POWER_DOMINIO_ABSOLUTO;
            enemy->power_cooldown = 4.0f;
            enemy->power_duration = 2.0f;
            enemy->master_power_switch_timer = 0.0f;
            break;
    }
    
    enemy->power_timer = enemy->power_cooldown;
    enemy->power_active = false;
    enemy->player_stunned = false;
    enemy->stun_timer = 0.0f;
    enemy->fog_intensity = 0.0f;
    enemy->fake_balls_active = false;
    
    enemy->visual_effect.active = false;
    enemy->visual_effect.timer = 0.0f;
    
    enemy->barrier.active = false;
    
    enemy->clone.active = false;
	enemy->pre_effect.active = false;
	enemy->pre_effect.timer = 0.0f;
	enemy->pre_effect.duration = 0.5f;
	enemy->pre_effect.oscillation_speed = 8.0f;

	enemy->electric_ray.active = false;
	enemy->electric_ray.timer = 0.0f;
	enemy->electric_ray.color_alternation_timer = 0.0f;
	enemy->electric_ray.is_white_phase = true;

	enemy->power_charging = false;
	enemy->power_charge_timer = 0.0f;
}

void enemy_update(Enemy* enemy, Ball* ball, Paddle* player_paddle, float delta_time) {
    if (!enemy || !ball || !player_paddle) return;

    enemy->ai_timer += delta_time;
    enemy->pattern_timer += delta_time;

    if (enemy->powerup_cooldown > 0) {
        enemy->powerup_cooldown -= delta_time;
    }
    if (enemy->special_move_cooldown > 0) {
        enemy->special_move_cooldown -= delta_time;
    }

    if (enemy->is_stunned) {
        enemy->stun_duration -= delta_time;
        if (enemy->stun_duration <= 0) {
            enemy->is_stunned = false;
        }
        return; // Não fazer mais nada se estiver stunned
    }

    enemy_update_ai(enemy, ball, player_paddle, delta_time);

    enemy_move_to_target(enemy, delta_time);

    enemy_update_movement_pattern(enemy, delta_time);
    
    enemy_update_powers(enemy, ball, player_paddle, delta_time);

    if (enemy->type >= ENEMY_BOSS_STORM) {
        enemy_apply_boss_ability(enemy, ball);
    }
}

void enemy_update_powers(Enemy* enemy, Ball* ball, Paddle* player_paddle, float delta_time) {
    if (!enemy || !ball || !player_paddle) return;
    
    enemy->power_timer -= delta_time;
    
    if (enemy->player_stunned) {
        enemy->stun_timer -= delta_time;
        if (enemy->stun_timer <= 0.0f) {
            enemy->player_stunned = false;
        }
    }
    
    if (enemy->pre_effect.active) {
        enemy->pre_effect.timer -= delta_time;
        
        if (enemy->pre_effect.effect_type == 1 || enemy->pre_effect.effect_type == 2 || 
            enemy->pre_effect.effect_type == 3 || enemy->pre_effect.effect_type == 6) {
            enemy->pre_effect.pos_x = ball->x;
            enemy->pre_effect.pos_y = ball->y;
        }
        
        if (enemy->pre_effect.timer <= 0.0f) {
            enemy->pre_effect.active = false;
            if (enemy->power_charging) {
                enemy_activate_power(enemy, ball, player_paddle);
                enemy->power_active = true;
                enemy->power_charging = false;
            }
        }
    }
    
    if (enemy->power_timer <= 0.0f && !enemy->power_active && !enemy->power_charging) {
        if (enemy->power_type == POWER_ERRO_BOBO || 
            enemy->power_type == POWER_BOLADA_ATORDOANTE ||
            enemy->power_type == POWER_BOLA_CAOTICA ||
            enemy->power_type == POWER_MURALHA_ACO ||
            enemy->power_type == POWER_TROCA_PERCURSO) {
            
            enemy->power_charging = true;
            enemy_start_pre_power_effect(enemy, ball, player_paddle);
        } else {
            enemy_activate_power(enemy, ball, player_paddle);
            enemy->power_active = true;
        }
        
        enemy->power_timer = enemy->power_cooldown;
    }
    
    if (enemy->power_active) {
        enemy_update_active_power(enemy, ball, player_paddle, delta_time);
    }
    
    enemy_update_visual_effects(enemy, delta_time);
}

static void enemy_start_pre_power_effect(Enemy* enemy, Ball* ball, Paddle* player_paddle) {
    switch (enemy->power_type) {
        case POWER_ERRO_BOBO:
            enemy->pre_effect.active = true;
            enemy->pre_effect.timer = 1.0f;  // Aumentado de 0.5f para 1.0f
            enemy->pre_effect.duration = 1.0f;
            enemy->pre_effect.effect_type = 1;
            enemy->pre_effect.pos_x = ball->x;
            enemy->pre_effect.pos_y = ball->y;
            enemy->pre_effect.color_r = 0.6f;
            enemy->pre_effect.color_g = 0.2f;
            enemy->pre_effect.color_b = 0.8f;
            enemy->pre_effect.color_a = 0.7f;
            enemy->pre_effect.radius = 25.0f;
            break;
            
        case POWER_BOLADA_ATORDOANTE:
            enemy->pre_effect.active = true;
            enemy->pre_effect.timer = 1.0f;  // Aumentado de 0.5f para 1.0f
            enemy->pre_effect.duration = 1.0f;
            enemy->pre_effect.effect_type = 2;
            enemy->pre_effect.pos_x = ball->x;
            enemy->pre_effect.pos_y = ball->y;
            enemy->pre_effect.color_r = 1.0f;
            enemy->pre_effect.color_g = 0.1f;
            enemy->pre_effect.color_b = 0.1f;
            enemy->pre_effect.color_a = 0.8f;
            enemy->pre_effect.radius = 30.0f;
            break;
            
        case POWER_BOLA_CAOTICA:
            enemy->pre_effect.active = true;
            enemy->pre_effect.timer = 1.0f;  // Aumentado de 0.5f para 1.0f
            enemy->pre_effect.duration = 1.0f;
            enemy->pre_effect.effect_type = 3;
            enemy->pre_effect.pos_x = ball->x;
            enemy->pre_effect.pos_y = ball->y;
            enemy->pre_effect.color_r = 0.1f;
            enemy->pre_effect.color_g = 0.8f;
            enemy->pre_effect.color_b = 0.1f;
            enemy->pre_effect.color_a = 0.7f;
            enemy->pre_effect.radius = 28.0f;
            break;
            
        case POWER_MURALHA_ACO:
            enemy->pre_effect.active = true;
            enemy->pre_effect.timer = 1.0f;  // Aumentado de 0.5f para 1.0f
            enemy->pre_effect.duration = 1.0f;
            enemy->pre_effect.effect_type = 4;
            enemy->pre_effect.pos_x = 350.0f;
            enemy->pre_effect.pos_y = 150.0f;
            enemy->pre_effect.color_r = 0.2f;
            enemy->pre_effect.color_g = 0.2f;
            enemy->pre_effect.color_b = 0.2f;
            enemy->pre_effect.color_a = 0.6f;
            enemy->pre_effect.radius = 0.0f;
            break;
            
        case POWER_TROCA_PERCURSO:
            enemy->pre_effect.active = true;
            enemy->pre_effect.timer = 1.0f;  // Aumentado de 0.5f para 1.0f
            enemy->pre_effect.duration = 1.0f;
            enemy->pre_effect.effect_type = 6;
            enemy->pre_effect.pos_x = ball->x;
            enemy->pre_effect.pos_y = ball->y;
            enemy->pre_effect.color_r = 1.0f;
            enemy->pre_effect.color_g = 0.0f;
            enemy->pre_effect.color_b = 1.0f;
            enemy->pre_effect.color_a = 0.9f;
            enemy->pre_effect.radius = 15.0f;
            break;
    }
}



void enemy_activate_power(Enemy* enemy, Ball* ball, Paddle* player_paddle) {
    switch (enemy->power_type) {
        case POWER_ERRO_BOBO:
            enemy_activate_erro_bobo(enemy, ball);
            break;
        case POWER_MURALHA_ACO:
            enemy_activate_muralha_aco(enemy);
            break;
        case POWER_BOLADA_ATORDOANTE:
            enemy_activate_bolada_atordoante(enemy, ball, player_paddle);
            break;
        case POWER_TROCA_PERCURSO:
            enemy_activate_troca_percurso(enemy, ball);
            break;
        case POWER_NEBLINA_DENSA:
            enemy_activate_neblina_densa(enemy);
            break;
        case POWER_CHOQUE_PARALISANTE:
            enemy_activate_choque_paralisante(enemy, player_paddle);
            break;
        case POWER_BOLA_CAOTICA:
            enemy_activate_bola_caotica(enemy, ball);
            break;
        case POWER_CLONE_SOMBRAS:
            enemy_activate_clone_sombras(enemy);
            break;
        case POWER_DOMINIO_ABSOLUTO:
            enemy_activate_dominio_absoluto(enemy, ball, player_paddle);
            break;
    }
}

void enemy_update_active_power(Enemy* enemy, Ball* ball, Paddle* player_paddle, float delta_time) {
    enemy->power_duration -= delta_time;
    
    if (enemy->power_duration <= 0.0f) {
        enemy->power_active = false;
        enemy->power_duration = 2.0f; // Resetar duração padrão
        
        enemy->barrier.active = false;
        enemy->clone.active = false;
        enemy->fake_balls_active = false;
        enemy->fog_intensity = 0.0f;
        return;
    }
    
	if (enemy->power_type == POWER_CHOQUE_PARALISANTE && enemy->power_active) {
		enemy_update_electric_ray(enemy, player_paddle, delta_time);
	}
    
    switch (enemy->power_type) {
        case POWER_MURALHA_ACO:
            enemy_update_muralha_aco(enemy, ball, delta_time);
            break;
        case POWER_NEBLINA_DENSA:
            enemy_update_neblina_densa(enemy, delta_time);
            break;
        case POWER_BOLA_CAOTICA:
            enemy_update_bola_caotica(enemy, ball, delta_time);
            break;
        case POWER_CLONE_SOMBRAS:
            enemy_update_clone_sombras(enemy, delta_time);
            break;
        case POWER_DOMINIO_ABSOLUTO:
            enemy_update_dominio_absoluto(enemy, ball, player_paddle, delta_time);
            break;
    }
}


void enemy_activate_erro_bobo(Enemy* enemy, Ball* ball) {
    float random_x = ((float)rand() / RAND_MAX - 0.5f) * 300.0f; // Triplicado
    float random_y = ((float)rand() / RAND_MAX - 0.5f) * 300.0f; // Triplicado
    
    ball->vx += random_x;
    ball->vy += random_y;
    
    for (int i = 0; i < 3; i++) {
        float extra_x = ((float)rand() / RAND_MAX - 0.5f) * 150.0f;
        float extra_y = ((float)rand() / RAND_MAX - 0.5f) * 150.0f;
        ball->vx += extra_x;
        ball->vy += extra_y;
    }
    
    enemy->visual_effect.active = true;
    enemy->visual_effect.timer = 1.5f; // Aumentado
    enemy->visual_effect.effect_type = 1;
    enemy->visual_effect.color_r = 1.0f;
    enemy->visual_effect.color_g = 1.0f;
    enemy->visual_effect.color_b = 0.0f;
    enemy->visual_effect.intensity = 2.0f; // Dobrado
}

void enemy_activate_muralha_aco(Enemy* enemy) {
    enemy->barrier.active = true;
    enemy->barrier.x = 350.0f; // Posição no meio da tela
    enemy->barrier.y = 150.0f; 
    enemy->barrier.width = 20.0f; 
    enemy->barrier.height = 300.0f; 
    enemy->barrier.timer = 8.0f; 
    enemy->barrier.alpha = 1.0f;
    
    enemy->visual_effect.active = true;
    enemy->visual_effect.timer = 8.0f;
    enemy->visual_effect.effect_type = 2;
    enemy->visual_effect.color_r = 0.7f;
    enemy->visual_effect.color_g = 0.7f;
    enemy->visual_effect.color_b = 0.9f;
    enemy->visual_effect.pos_x = enemy->barrier.x;
    enemy->visual_effect.pos_y = enemy->barrier.y;
    enemy->visual_effect.intensity = 2.0f;
}

void enemy_activate_bolada_atordoante(Enemy* enemy, Ball* ball, Paddle* player_paddle) {
    float speed_multiplier = 4.0f; // Aumentado drasticamente
    ball->vx *= speed_multiplier;
    ball->vy *= speed_multiplier;
    
    enemy->stun_timer = 2.0f; // Quadruplicado
    
    enemy->visual_effect.active = true;
    enemy->visual_effect.timer = 2.0f;
    enemy->visual_effect.effect_type = 3;
    enemy->visual_effect.color_r = 1.0f;
    enemy->visual_effect.color_g = 0.1f; // Mais vermelho
    enemy->visual_effect.color_b = 0.1f;
    enemy->visual_effect.intensity = 3.0f; // Dobrado
}

void enemy_activate_troca_percurso(Enemy* enemy, Ball* ball) {
    float speed = sqrtf(ball->vx * ball->vx + ball->vy * ball->vy);
    
    float angle = M_PI + ((float)rand() / RAND_MAX - 0.5f) * M_PI * 0.6f; // Ângulo entre 180° ± 54°
    
    ball->vx = cosf(angle) * speed * 1.3f; // Aumenta velocidade também
    ball->vy = sinf(angle) * speed * 1.3f;
    
    if (ball->vx > 0) {
        ball->vx = -fabs(ball->vx);
    }
    
    enemy->visual_effect.active = true;
    enemy->visual_effect.timer = 1.0f;
    enemy->visual_effect.effect_type = 4;
    enemy->visual_effect.color_r = 0.3f;
    enemy->visual_effect.color_g = 0.6f;
    enemy->visual_effect.color_b = 1.0f;
    enemy->visual_effect.pos_x = ball->x;
    enemy->visual_effect.pos_y = ball->y;
    enemy->visual_effect.intensity = 2.5f;
}

void enemy_activate_neblina_densa(Enemy* enemy) {
    enemy->fog_intensity = 0.95f; // Quase opaca
    
    enemy->visual_effect.active = true;
    enemy->visual_effect.timer = 12.0f; // Triplicado o tempo
    enemy->visual_effect.effect_type = 5;
    enemy->visual_effect.color_r = 0.9f; // Mais densa
    enemy->visual_effect.color_g = 0.9f;
    enemy->visual_effect.color_b = 0.9f;
    enemy->visual_effect.color_a = 0.9f; // Muito mais opaca
    enemy->visual_effect.intensity = 3.0f;
}

void enemy_activate_choque_paralisante(Enemy* enemy, Paddle* player_paddle) {
    enemy->player_stunned = true;
    enemy->stun_timer = 3.0f;
    
    enemy->electric_ray.active = true;
    enemy->electric_ray.timer = 3.0f;
    enemy->electric_ray.color_alternation_timer = 0.0f;
    enemy->electric_ray.is_white_phase = true;
    
    enemy->electric_ray.start_x = enemy->paddle.x;
    enemy->electric_ray.start_y = enemy->paddle.y + enemy->paddle.height / 2;
    enemy->electric_ray.end_x = player_paddle->x + player_paddle->width;
    enemy->electric_ray.end_y = player_paddle->y + player_paddle->height / 2;
    
    enemy->visual_effect.active = true;
    enemy->visual_effect.timer = 3.0f;
    enemy->visual_effect.effect_type = 6;
    enemy->visual_effect.color_r = 1.0f;
    enemy->visual_effect.color_g = 1.0f;
    enemy->visual_effect.color_b = 0.0f;
    enemy->visual_effect.pos_x = player_paddle->x;
    enemy->visual_effect.pos_y = player_paddle->y;
    enemy->visual_effect.intensity = 4.0f;
}

static void enemy_update_electric_ray(Enemy* enemy, Paddle* player_paddle, float delta_time) {
    if (!enemy->electric_ray.active) return;
    
    enemy->electric_ray.timer -= delta_time;
    if (enemy->electric_ray.timer <= 0.0f) {
        enemy->electric_ray.active = false;
        return;
    }
    
    enemy->electric_ray.start_x = enemy->paddle.x;
    enemy->electric_ray.start_y = enemy->paddle.y + enemy->paddle.height / 2;
    enemy->electric_ray.end_x = player_paddle->x + player_paddle->width;
    enemy->electric_ray.end_y = player_paddle->y + player_paddle->height / 2;
    
    enemy->electric_ray.color_alternation_timer += delta_time;
    if (enemy->electric_ray.color_alternation_timer >= 0.1f) {
        enemy->electric_ray.is_white_phase = !enemy->electric_ray.is_white_phase;
        enemy->electric_ray.color_alternation_timer = 0.0f;
    }
}

void enemy_activate_bola_caotica(Enemy* enemy, Ball* ball) {
    enemy->fake_balls_active = true;
    
    for (int i = 0; i < 2; i++) {
        enemy->fake_balls[i] = *ball;
        
        float angle_offset = (i + 1) * (M_PI * 2.0f / 2.0f);
        enemy->fake_balls[i].x += cosf(angle_offset) * 40.0f;
        enemy->fake_balls[i].y += sinf(angle_offset) * 40.0f;
        
        float speed = sqrtf(ball->vx * ball->vx + ball->vy * ball->vy);
        float trajectory_angle = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
        
        enemy->fake_balls[i].vx = cosf(trajectory_angle) * speed;
        enemy->fake_balls[i].vy = sinf(trajectory_angle) * speed;
        
        float speed_variation = 0.5f + ((float)rand() / RAND_MAX) * 1.0f; // 0.5x a 1.5x
        enemy->fake_balls[i].vx *= speed_variation;
        enemy->fake_balls[i].vy *= speed_variation;
    }
    
    enemy->visual_effect.active = true;
    enemy->visual_effect.timer = 8.0f; // Aumentado
    enemy->visual_effect.effect_type = 7;
    enemy->visual_effect.color_r = 0.9f;
    enemy->visual_effect.color_g = 0.1f;
    enemy->visual_effect.color_b = 0.9f;
    enemy->visual_effect.intensity = 2.5f;
}
void enemy_activate_clone_sombras(Enemy* enemy) {
    enemy->clone.active = true;
    enemy->clone.x = enemy->paddle.x - 60.0f; // Posiciona na frente do inimigo
    enemy->clone.y = enemy->paddle.y;
    enemy->clone.width = enemy->paddle.width;   
    enemy->clone.height = enemy->paddle.height; 
    enemy->clone.move_speed = 150.0f;
    enemy->clone.alpha = 0.7f;
    enemy->clone.target_y = 200.0f + ((float)rand() / RAND_MAX) * 200.0f;
    
    enemy->visual_effect.active = true;
    enemy->visual_effect.timer = 5.0f;
    enemy->visual_effect.effect_type = 8;
    enemy->visual_effect.color_r = 0.3f;
    enemy->visual_effect.color_g = 0.3f;
    enemy->visual_effect.color_b = 0.3f;
}

void enemy_activate_dominio_absoluto(Enemy* enemy, Ball* ball, Paddle* player_paddle) {
    EnemyPowerType powers[] = {
        POWER_ERRO_BOBO, POWER_MURALHA_ACO, POWER_BOLADA_ATORDOANTE,
        POWER_TROCA_PERCURSO, POWER_NEBLINA_DENSA, POWER_CHOQUE_PARALISANTE,
        POWER_BOLA_CAOTICA, POWER_CLONE_SOMBRAS
    };
    
    int index = rand() % 8;
    enemy->current_master_power = powers[index];
    
    EnemyPowerType original_power = enemy->power_type;
    enemy->power_type = enemy->current_master_power;
    enemy_activate_power(enemy, ball, player_paddle);
    enemy->power_type = original_power;
    
    enemy->visual_effect.active = true;
    enemy->visual_effect.timer = 2.0f;
    enemy->visual_effect.effect_type = 9;
    enemy->visual_effect.intensity = 1.5f;
}


void enemy_update_muralha_aco(Enemy* enemy, Ball* ball, float delta_time) {
    if (!enemy->barrier.active) return;
    
    enemy->barrier.timer -= delta_time;
    if (enemy->barrier.timer <= 0.0f) {
        enemy->barrier.active = false;
        return;
    }
    
    if (ball->x + ball->radius >= enemy->barrier.x &&
        ball->x - ball->radius <= enemy->barrier.x + enemy->barrier.width &&
        ball->y + ball->radius >= enemy->barrier.y &&
        ball->y - ball->radius <= enemy->barrier.y + enemy->barrier.height) {
        
        if (ball->vx > 0) {
            ball->vx = -fabs(ball->vx) * 1.2f;
            ball->x = enemy->barrier.x - ball->radius;
            printf("Muralha rebateu bola de volta para o jogador!\n");
        } else {
            ball->x = enemy->barrier.x + enemy->barrier.width + ball->radius;
            printf("Bola atravessou a muralha pelo lado do inimigo!\n");
        }
        
        enemy->visual_effect.intensity = 3.0f;
    }
}

void enemy_update_neblina_densa(Enemy* enemy, float delta_time) {
    enemy->fog_intensity = fmaxf(0.0f, enemy->fog_intensity - delta_time * 0.08f); // Muito mais lento
}

void enemy_update_bola_caotica(Enemy* enemy, Ball* ball, float delta_time) {
    if (!enemy->fake_balls_active) return;
    
    for (int i = 0; i < 2; i++) {
        enemy->fake_balls[i].x += enemy->fake_balls[i].vx * delta_time;
        enemy->fake_balls[i].y += enemy->fake_balls[i].vy * delta_time;
        
        if (enemy->fake_balls[i].y <= 0 || enemy->fake_balls[i].y >= 600) {
            enemy->fake_balls[i].vy = -enemy->fake_balls[i].vy;
        }
        
        if (enemy->fake_balls[i].x <= 0 || enemy->fake_balls[i].x >= 800) {
            enemy->fake_balls[i].vx = -enemy->fake_balls[i].vx;
        }
        
        if ((int)(enemy->ai_timer * 10) % 30 == 0) {
            float random_change_x = ((float)rand() / RAND_MAX - 0.5f) * 50.0f;
            float random_change_y = ((float)rand() / RAND_MAX - 0.5f) * 50.0f;
            enemy->fake_balls[i].vx += random_change_x;
            enemy->fake_balls[i].vy += random_change_y;
        }
    }
}

void enemy_update_clone_sombras(Enemy* enemy, float delta_time) {
    if (!enemy->clone.active) return;
    
    float diff = enemy->clone.target_y - enemy->clone.y;
    if (fabs(diff) < 5.0f) {
        enemy->clone.target_y = 100.0f + ((float)rand() / RAND_MAX) * 400.0f;
    }
    
    if (diff > 0) {
        enemy->clone.y += enemy->clone.move_speed * delta_time;
    } else {
        enemy->clone.y -= enemy->clone.move_speed * delta_time;
    }
    
    if (enemy->clone.y < 50.0f) enemy->clone.y = 50.0f;
    if (enemy->clone.y > 550.0f) enemy->clone.y = 550.0f;
}

void enemy_update_dominio_absoluto(Enemy* enemy, Ball* ball, Paddle* player_paddle, float delta_time) {
    enemy->master_power_switch_timer -= delta_time;
    
    if (enemy->master_power_switch_timer <= 0.0f) {
        enemy_activate_dominio_absoluto(enemy, ball, player_paddle);
        enemy->master_power_switch_timer = 1.0f;
    }
}

void enemy_update_visual_effects(Enemy* enemy, float delta_time) {
    if (enemy->visual_effect.active) {
        enemy->visual_effect.timer -= delta_time;
        if (enemy->visual_effect.timer <= 0.0f) {
            enemy->visual_effect.active = false;
        }
        
        float time_ratio = enemy->visual_effect.timer / enemy->visual_effect.duration;
        enemy->visual_effect.intensity = time_ratio;
    }
}

void enemy_reset(Enemy* enemy) {
    if (!enemy) return;

    enemy->health = enemy->max_health;
    enemy->ai_state = AI_STATE_TRACKING;
    enemy->ai_timer = 0.0f;
    enemy->target_y = 300.0f;
    enemy->is_stunned = false;
    enemy->is_enraged = false;
    enemy->in_special_move = false;
    enemy->powerup_cooldown = 0.0f;
    enemy->special_move_cooldown = 0.0f;
    enemy->pattern_timer = 0.0f;

    enemy->hits_made = 0;
    enemy->hits_received = 0;
    enemy->special_moves_used = 0;
    enemy->damage_dealt = 0.0f;
    enemy->accuracy = 0.0f;
}

void enemy_take_damage(Enemy* enemy, int damage) {
    if (!enemy) return;

    enemy->health -= damage;
    if (enemy->health < 0) {
        enemy->health = 0;
    }

    if (enemy->health < enemy->max_health * 0.3f && !enemy->is_enraged) {
        enemy_enrage(enemy, 1.5f, 10.0f);
    }
}

void enemy_heal(Enemy* enemy, int health) {
    if (!enemy) return;

    enemy->health += health;
    if (enemy->health > enemy->max_health) {
        enemy->health = enemy->max_health;
    }
}

bool enemy_is_alive(Enemy* enemy) {
    return enemy && enemy->health > 0;
}

void enemy_update_ai(Enemy* enemy, Ball* ball, Paddle* player_paddle, float delta_time) {
    if (!enemy || !ball || !player_paddle) return;

    enemy_calculate_target(enemy, ball, player_paddle);

    switch (enemy->ai_state) {
        case AI_STATE_TRACKING:
            enemy_update_tracking(enemy, ball, delta_time);
            break;
        case AI_STATE_ATTACKING:
            enemy_update_attacking(enemy, ball, delta_time);
            break;
        case AI_STATE_DEFENDING:
            enemy_update_defending(enemy, ball, delta_time);
            break;
        case AI_STATE_SPECIAL_MOVE:
            enemy_update_special_move(enemy, ball, delta_time);
            break;
        case AI_STATE_RECOVERING:
            enemy->target_y = 300.0f;
            if (fabs(enemy->paddle.y - 300.0f) < 10.0f) {
                enemy_set_ai_state(enemy, AI_STATE_TRACKING);
            }
            break;
        case AI_STATE_STUNNED:
            break;
    }
}

void enemy_calculate_target(Enemy* enemy, Ball* ball, Paddle* player_paddle) {
    if (!enemy || !ball) return;

    float predicted_y = ball->y;

    if (enemy_ball_coming_to_side(enemy, ball)) {
        enemy_predict_ball_position(enemy, ball, &predicted_y);
    }

    float center_pull = (300.0f - predicted_y) * enemy->center_bias;
    predicted_y += center_pull;

    enemy_apply_error(enemy, &predicted_y);

    float pattern_offset = enemy_calculate_pattern_offset(enemy, enemy->movement_pattern);
    predicted_y += pattern_offset;

    if (predicted_y < 50.0f) predicted_y = 50.0f;
    if (predicted_y > 550.0f) predicted_y = 550.0f;

    enemy->target_y = predicted_y;
}

void enemy_move_to_target(Enemy* enemy, float delta_time) {
    if (!enemy) return;

    float current_y = enemy->paddle.y;
    float diff = enemy->target_y - current_y;

    if (fabs(diff) > 2.0f) {
        float move_speed = enemy->move_speed_current * delta_time;

        if (enemy->is_enraged) {
            move_speed *= enemy->rage_multiplier;
        }

        if (diff > 0) {
            enemy->paddle.y += move_speed;
        } else {
            enemy->paddle.y -= move_speed;
        }

        if (enemy->paddle.y < 50.0f) enemy->paddle.y = 50.0f;
        if (enemy->paddle.y > 550.0f) enemy->paddle.y = 550.0f;
    }
}

void enemy_predict_ball_position(Enemy* enemy, Ball* ball, float* predicted_y) {
    if (!enemy || !ball || !predicted_y) return;

    float time_to_intercept = enemy_calculate_interception_time(enemy, ball);

    *predicted_y = ball->y + (ball->vy * time_to_intercept);

    float error = (1.0f - enemy->prediction_accuracy) * 100.0f;
    float random_error = ((float)rand() / RAND_MAX - 0.5f) * error;
    *predicted_y += random_error;
}

void enemy_adjust_difficulty(Enemy* enemy, float player_performance) {
    if (!enemy) return;

    if (player_performance > 0.8f) {
        enemy->prediction_accuracy = fminf(enemy->prediction_accuracy + 0.05f, 0.95f);
        enemy->reaction_time = fmaxf(enemy->reaction_time - 0.02f, 0.1f);
        enemy->error_rate = fmaxf(enemy->error_rate - 0.01f, 0.01f);
    } else if (player_performance < 0.4f) {
        enemy->prediction_accuracy = fmaxf(enemy->prediction_accuracy - 0.03f, 0.2f);
        enemy->reaction_time = fminf(enemy->reaction_time + 0.05f, 1.0f);
        enemy->error_rate = fminf(enemy->error_rate + 0.02f, 0.4f);
    }
}

void enemy_set_ai_state(Enemy* enemy, AIState state) {
    if (!enemy) return;

    enemy->ai_state = state;
    enemy->ai_timer = 0.0f;
}

void enemy_update_tracking(Enemy* enemy, Ball* ball, float delta_time) {
    if (!enemy || !ball) return;

    if (enemy_ball_coming_to_side(enemy, ball)) {
        if (enemy->aggression > 0.6f && (rand() % 100) < 30) {
            enemy_set_ai_state(enemy, AI_STATE_ATTACKING);
        } else {
            enemy_set_ai_state(enemy, AI_STATE_DEFENDING);
        }
    }

    if (enemy_can_use_special(enemy) && (rand() % 100) < 10) {
        enemy_set_ai_state(enemy, AI_STATE_SPECIAL_MOVE);
    }
}

void enemy_update_attacking(Enemy* enemy, Ball* ball, float delta_time) {
    if (!enemy || !ball) return;

    enemy->move_speed_current = enemy->move_speed_base * 1.3f;

    if (enemy->ai_timer > 2.0f) {
        enemy_set_ai_state(enemy, AI_STATE_TRACKING);
        enemy->move_speed_current = enemy->move_speed_base;
    }
}

void enemy_update_defending(Enemy* enemy, Ball* ball, float delta_time) {
    if (!enemy || !ball) return;

    enemy->move_speed_current = enemy->move_speed_base * 0.8f;
    enemy->center_bias = 0.6f; // Mais tendência ao centro

    if (enemy->ai_timer > 1.5f) {
        enemy_set_ai_state(enemy, AI_STATE_TRACKING);
        enemy->move_speed_current = enemy->move_speed_base;
        enemy->center_bias = 0.3f;
    }
}

void enemy_update_special_move(Enemy* enemy, Ball* ball, float delta_time) {
    if (!enemy || !ball) return;

    if (!enemy->in_special_move) {
        enemy_use_special_move(enemy, ball);
        enemy->in_special_move = true;
    }

    if (enemy->ai_timer > enemy->special_duration) {
        enemy->in_special_move = false;
        enemy->special_move_cooldown = 5.0f;
        enemy_set_ai_state(enemy, AI_STATE_TRACKING);
    }
}

bool enemy_can_use_special(Enemy* enemy) {
    if (!enemy) return false;

    return enemy->can_use_powerups &&
           enemy->special_move_cooldown <= 0 &&
           !enemy->in_special_move &&
           !enemy->is_stunned;
}

void enemy_use_special_move(Enemy* enemy, Ball* ball) {
    if (!enemy || !ball) return;

    switch (enemy->type) {
        case ENEMY_BOSS_STORM:
            ball->vx *= 1.5f;
            ball->vy *= 1.5f;
            break;

        case ENEMY_BOSS_LIGHTNING:
            enemy->move_speed_current = enemy->move_speed_base * 3.0f;
            break;

        case ENEMY_BOSS_CHAOS:
            enemy_set_movement_pattern(enemy, PATTERN_RANDOM);
            break;

        case ENEMY_BOSS_MASTER:
            enemy->prediction_accuracy = 1.0f;
            enemy->error_rate = 0.0f;
            break;

        default:
            enemy->move_speed_current = enemy->move_speed_base * 1.5f;
            break;
    }

    enemy->special_moves_used++;
}

void enemy_activate_powerup(Enemy* enemy, PowerupType type) {
    if (!enemy) return;

    switch (type) {
        case POWERUP_SPEED_BOOST:
            enemy->move_speed_current = enemy->move_speed_base * 1.5f;
            break;
        case POWERUP_SIZE_BOOST:  // Mudou de SIZE_INCREASE para SIZE_BOOST
            break;
        case POWERUP_ENERGY_SHIELD:  // Mudou de SHIELD para ENERGY_SHIELD
            break;
        default:
            break;
    }

    enemy->powerup_cooldown = 3.0f;
}

void enemy_apply_boss_ability(Enemy* enemy, Ball* ball) {
    if (!enemy || !ball) return;

    switch (enemy->type) {
        case ENEMY_BOSS_STORM:
            if ((int)(enemy->ai_timer * 10) % 50 == 0) {
                enemy_heal(enemy, 1);
            }
            break;

        case ENEMY_BOSS_LIGHTNING:
            if (enemy->health < enemy->max_health * 0.5f) {
                enemy->move_speed_current = enemy->move_speed_base * 1.2f;
            }
            break;

        case ENEMY_BOSS_CHAOS:
            if ((int)(enemy->ai_timer * 10) % 100 == 0) {
                enemy->movement_pattern = rand() % 8;
            }
            break;

        case ENEMY_BOSS_MASTER:
            if ((int)(enemy->ai_timer * 10) % 75 == 0) {
                enemy->prediction_accuracy = fminf(enemy->prediction_accuracy + 0.01f, 0.98f);
            }
            break;
    }
}

void enemy_set_movement_pattern(Enemy* enemy, MovementPattern pattern) {
    if (!enemy) return;

    enemy->movement_pattern = pattern;
    enemy->pattern_timer = 0.0f;
}

void enemy_update_movement_pattern(Enemy* enemy, float delta_time) {
    if (!enemy) return;

    enemy->pattern_timer += delta_time;
}

float enemy_calculate_pattern_offset(Enemy* enemy, MovementPattern pattern) {
    if (!enemy) return 0.0f;

    float t = enemy->pattern_timer;
    float offset = 0.0f;

    switch (pattern) {
        case PATTERN_STRAIGHT:
            offset = 0.0f;
            break;

        case PATTERN_SINE_WAVE:
            offset = enemy->pattern_amplitude * sinf(enemy->pattern_frequency * t);
            break;

        case PATTERN_TRIANGLE_WAVE: {
            float period = 2.0f * M_PI / enemy->pattern_frequency;
            float cycle = fmodf(t, period);
            offset = enemy->pattern_amplitude * (cycle < period/2 ?
                (4.0f * cycle / period - 1.0f) :
                (3.0f - 4.0f * cycle / period));
            break;
        }

        case PATTERN_CIRCULAR:
            offset = enemy->pattern_amplitude * cosf(enemy->pattern_frequency * t);
            break;

        case PATTERN_ZIGZAG:
            offset = enemy->pattern_amplitude * (sinf(enemy->pattern_frequency * t) > 0 ? 1.0f : -1.0f);
            break;

        case PATTERN_RANDOM:
            offset = enemy->pattern_amplitude * ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
            break;

        case PATTERN_ADAPTIVE:
            offset = 0.0f; // Implementar quando tiver acesso ao jogador
            break;

        case PATTERN_MIRROR_PLAYER:
            offset = 0.0f; // Implementar quando tiver acesso ao jogador
            break;
    }

    return offset;
}

void enemy_stun(Enemy* enemy, float duration) {
    if (!enemy) return;

    enemy->is_stunned = true;
    enemy->stun_duration = duration;
    enemy_set_ai_state(enemy, AI_STATE_STUNNED);
}

void enemy_enrage(Enemy* enemy, float multiplier, float duration) {
    if (!enemy) return;

    enemy->is_enraged = true;
    enemy->rage_multiplier = multiplier;
}

void enemy_slow(Enemy* enemy, float multiplier, float duration) {
    if (!enemy) return;

    enemy->move_speed_current = enemy->move_speed_base * multiplier;
}

void enemy_confuse(Enemy* enemy, float duration) {
    if (!enemy) return;

    enemy->error_rate = fminf(enemy->error_rate + 0.3f, 0.8f);
}

void enemy_increase_difficulty(Enemy* enemy, float amount) {
    if (!enemy) return;

    enemy->difficulty_multiplier += amount;
    enemy->prediction_accuracy = fminf(enemy->prediction_accuracy + amount * 0.1f, 0.95f);
    enemy->reaction_time = fmaxf(enemy->reaction_time - amount * 0.05f, 0.1f);
    enemy->error_rate = fmaxf(enemy->error_rate - amount * 0.05f, 0.01f);
}

void enemy_decrease_difficulty(Enemy* enemy, float amount) {
    if (!enemy) return;

    enemy->difficulty_multiplier -= amount;
    enemy->prediction_accuracy = fmaxf(enemy->prediction_accuracy - amount * 0.1f, 0.2f);
    enemy->reaction_time = fminf(enemy->reaction_time + amount * 0.05f, 1.0f);
    enemy->error_rate = fminf(enemy->error_rate + amount * 0.05f, 0.4f);
}

float enemy_get_difficulty_rating(Enemy* enemy) {
    if (!enemy) return 1.0f;

    return enemy->difficulty_multiplier;
}

void enemy_balance_difficulty(Enemy* enemy, float target_win_rate) {
    if (!enemy) return;

    float current_performance = (float)enemy->hits_made / fmaxf(enemy->hits_made + enemy->hits_received, 1.0f);

    if (current_performance > target_win_rate + 0.1f) {
        enemy_decrease_difficulty(enemy, 0.1f);
    } else if (current_performance < target_win_rate - 0.1f) {
        enemy_increase_difficulty(enemy, 0.1f);
    }
}

float enemy_get_distance_to_ball(Enemy* enemy, Ball* ball) {
    if (!enemy || !ball) return 0.0f;

    float dx = ball->x - enemy->paddle.x;
    float dy = ball->y - enemy->paddle.y;
    return sqrtf(dx*dx + dy*dy);
}

bool enemy_ball_coming_to_side(Enemy* enemy, Ball* ball) {
    if (!enemy || !ball) return false;

    return (ball->vx > 0 && enemy->paddle.x > 400.0f) ||
           (ball->vx < 0 && enemy->paddle.x < 400.0f);
}

float enemy_calculate_interception_time(Enemy* enemy, Ball* ball) {
    if (!enemy || !ball) return 0.0f;

    float distance_x = fabs(ball->x - enemy->paddle.x);
    return distance_x / fabs(ball->vx);
}

void enemy_apply_error(Enemy* enemy, float* target_y) {
    if (!enemy || !target_y) return;

    if ((float)rand() / RAND_MAX < enemy->error_rate) {
        float error = ((float)rand() / RAND_MAX - 0.5f) * 100.0f;
        *target_y += error;
    }
}
