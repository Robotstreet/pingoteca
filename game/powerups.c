#include "powerups.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159265359f
#define MAX_CLONE_BALLS 4

typedef struct {
    float x, y;
    float vx, vy;
    bool active;
    float lifetime;
} CloneBall;

static CloneBall clone_balls[MAX_CLONE_BALLS];

static const float powerup_base_durations[] = {
    [POWERUP_MULTI_BALL] = 30.0f,
    [POWERUP_TRIPLE_HIT] = 15.0f,
    [POWERUP_TURBO_SMASH] = 10.0f,
    [POWERUP_ENERGY_SHIELD] = 20.0f,
    [POWERUP_SIZE_BOOST] = 15.0f,
    [POWERUP_SPEED_BOOST] = 12.0f,
    [POWERUP_MAGNETIC_PADDLE] = 18.0f,
    [POWERUP_FIRE_BALL] = 25.0f,
    [POWERUP_GHOST_BALL] = 8.0f,
    [POWERUP_SLOW_MOTION] = 10.0f,
    [POWERUP_PRECISION_AIM] = 20.0f,
    [POWERUP_LIFE_STEAL] = 1.0f,
    [POWERUP_CLONE_BALL] = 15.0f,
    [POWERUP_INVISIBLE_BALL] = 12.0f,
    [POWERUP_ZIGZAG_BALL] = 20.0f,
    [POWERUP_GRAVITY_BALL] = 18.0f,
    [POWERUP_TELEPORT_BALL] = 10.0f,
    [POWERUP_PHASE_BALL] = 8.0f,
    [POWERUP_HOMING_BALL] = 15.0f,
    [POWERUP_SPLIT_BALL] = 5.0f,
    [POWERUP_CURVE_BALL] = 20.0f,
    [POWERUP_SPIRAL_BALL] = 18.0f,
    [POWERUP_BOUNCY_BALL] = 25.0f,
    [POWERUP_HEAVY_BALL] = 15.0f,
    [POWERUP_LIGHT_BALL] = 15.0f,
    [POWERUP_MAGNETIC_BALL] = 18.0f,
    [POWERUP_ELECTRIC_BALL] = 12.0f,
    [POWERUP_ICE_BALL] = 10.0f,
    [POWERUP_POISON_BALL] = 20.0f,
    [POWERUP_EXPLOSIVE_BALL] = 8.0f,
    [POWERUP_ENEMY_SLOW] = 15.0f,
    [POWERUP_ENEMY_BLIND] = 8.0f,
    [POWERUP_ENEMY_CONFUSION] = 12.0f,
    [POWERUP_ENEMY_FREEZE] = 5.0f,
    [POWERUP_ENEMY_SHRINK] = 20.0f,
    [POWERUP_ENEMY_DIZZY] = 10.0f,
    [POWERUP_PLAYER_SLOW] = 8.0f,
    [POWERUP_PLAYER_BLIND] = 5.0f,
    [POWERUP_PLAYER_CONFUSION] = 8.0f,
    [POWERUP_PLAYER_SHRINK] = 10.0f,
    [POWERUP_PLAYER_DIZZY] = 8.0f,
    [POWERUP_FOG_SCREEN] = 15.0f,
    [POWERUP_REVERSE_GRAVITY] = 20.0f,
    [POWERUP_SCREEN_SHAKE] = 5.0f,
    [POWERUP_COLOR_BLIND] = 12.0f,
    [POWERUP_DISCO_LIGHTS] = 30.0f,
    [POWERUP_MIRROR_WORLD] = 15.0f,
    [POWERUP_UPSIDE_DOWN] = 20.0f,
    [POWERUP_STROBE_EFFECT] = 10.0f,
    [POWERUP_RAIN_EFFECT] = 25.0f,
    [POWERUP_SNOW_EFFECT] = 25.0f
};

void powerups_init(PowerupSystem* system) {
    if (!system) return;
    
    memset(system, 0, sizeof(PowerupSystem));
    
    system->spawn_interval = 15.0f;  // 15 segundos entre spawns
    system->spawning_enabled = true;
    system->duration_multiplier = 1.0f;
    system->spawn_rate_multiplier = 1.0f;
    system->max_simultaneous = MAX_ACTIVE_POWERUPS;
    system->allow_stacking = false;
    
    memset(clone_balls, 0, sizeof(clone_balls));
}

void powerups_cleanup(PowerupSystem* system) {
    if (!system) return;
    
    powerups_clear_all(system);
    powerups_clear_spawns(system);
}

void powerups_update(PowerupSystem* system, float delta_time) {
    if (!system) return;
    
    for (int i = 0; i < system->active_count; i++) {
        ActivePowerup* powerup = &system->active_powerups[i];
        powerup->remaining_time -= delta_time;
        
        if (powerup->remaining_time <= 0.0f) {
            system->active_powerups[i] = system->active_powerups[system->active_count - 1];
            system->active_count--;
            i--; // Verificar novamente a mesma posição
        }
    }
    
    for (int i = 0; i < system->spawn_count; i++) {
        PowerupSpawn* spawn = &system->spawns[i];
        if (!spawn->active) continue;
        
        spawn->lifetime -= delta_time;
        spawn->animation_time += delta_time;
        spawn->pulse_phase += delta_time * 2.0f; // 2 Hz pulse
        
        if (spawn->lifetime <= 0.0f) {
            spawn->active = false;
        }
    }
    
    if (system->spawning_enabled) {
        system->spawn_timer += delta_time;
        if (system->spawn_timer >= system->spawn_interval * system->spawn_rate_multiplier) {
            system->spawn_timer = 0.0f;
            
            float x = 200 + (rand() % 400);  // Área central
            float y = 100 + (rand() % 400);
            powerups_spawn_random(system, x, y);
        }
    }
    
    for (int i = 0; i < MAX_CLONE_BALLS; i++) {
        if (clone_balls[i].active) {
            clone_balls[i].x += clone_balls[i].vx * delta_time;
            clone_balls[i].y += clone_balls[i].vy * delta_time;
            clone_balls[i].lifetime -= delta_time;
            
            if (clone_balls[i].lifetime <= 0.0f) {
                clone_balls[i].active = false;
            }
        }
    }
}

void powerups_render_effects(PowerupSystem* system) {
    if (!system) return;
    
    for (int i = 0; i < system->active_count; i++) {
        ActivePowerup* powerup = &system->active_powerups[i];
        
        switch (powerup->type) {
            case POWERUP_FIRE_BALL:
                break;
                
            case POWERUP_DISCO_LIGHTS:
                break;
                
            case POWERUP_RAIN_EFFECT:
                break;
                
            case POWERUP_SNOW_EFFECT:
                break;
                
            default:
                break;
        }
    }
    
    for (int i = 0; i < system->spawn_count; i++) {
        PowerupSpawn* spawn = &system->spawns[i];
        if (!spawn->active) continue;
        
        float pulse = 0.8f + 0.2f * sinf(spawn->pulse_phase);
        
    }
}

void powerups_activate(PowerupSystem* system, PowerupType type, float duration, int target) {
    if (!system || system->active_count >= MAX_ACTIVE_POWERUPS) return;
    
    if (!system->allow_stacking) {
        for (int i = 0; i < system->active_count; i++) {
            if (system->active_powerups[i].type == type && 
                system->active_powerups[i].target == target) {
                system->active_powerups[i].remaining_time = duration * system->duration_multiplier;
                return;
            }
        }
    }
    
    ActivePowerup* powerup = &system->active_powerups[system->active_count];
    powerup->type = type;
    powerup->duration = duration * system->duration_multiplier;
    powerup->remaining_time = powerup->duration;
    powerup->target = target;
    powerup->intensity = 1.0f;
    powerup->persistent = false;
    
    switch (type) {
        case POWERUP_MULTI_BALL:
            powerup->data.multi_ball.extra_balls = 2;
            break;
            
        case POWERUP_SPEED_BOOST:
            powerup->data.speed_boost.speed_multiplier = 1.5f;
            break;
            
        case POWERUP_SIZE_BOOST:
            powerup->data.size_boost.size_multiplier = 1.3f;
            break;
            
        case POWERUP_CLONE_BALL:
            powerup->data.clone_ball.clone_count = 3;
            powerup->data.clone_ball.clone_offset = 20.0f;
            break;
            
        case POWERUP_ZIGZAG_BALL:
            powerup->data.zigzag.amplitude = 50.0f;
            powerup->data.zigzag.frequency = 2.0f;
            break;
            
        case POWERUP_CURVE_BALL:
            powerup->data.curve.strength = 0.3f;
            break;
            
        case POWERUP_SPIRAL_BALL:
            powerup->data.spiral.radius = 30.0f;
            break;
            
        case POWERUP_TELEPORT_BALL:
            powerup->data.teleport.interval = 2.0f;
            break;
            
        default:
            break;
    }
    
    system->active_count++;
}

void powerups_deactivate(PowerupSystem* system, PowerupType type, int target) {
    if (!system) return;
    
    for (int i = 0; i < system->active_count; i++) {
        if (system->active_powerups[i].type == type && 
            system->active_powerups[i].target == target) {
            system->active_powerups[i] = system->active_powerups[system->active_count - 1];
            system->active_count--;
            break;
        }
    }
}

void powerups_clear_all(PowerupSystem* system) {
    if (!system) return;
    system->active_count = 0;
}

bool powerups_is_active(PowerupSystem* system, PowerupType type, int target) {
    if (!system) return false;
    
    for (int i = 0; i < system->active_count; i++) {
        if (system->active_powerups[i].type == type && 
            system->active_powerups[i].target == target) {
            return true;
        }
    }
    return false;
}

float powerups_get_remaining_time(PowerupSystem* system, PowerupType type, int target) {
    if (!system) return 0.0f;
    
    for (int i = 0; i < system->active_count; i++) {
        if (system->active_powerups[i].type == type && 
            system->active_powerups[i].target == target) {
            return system->active_powerups[i].remaining_time;
        }
    }
    return 0.0f;
}

int powerups_get_active_count(PowerupSystem* system) {
    return system ? system->active_count : 0;
}

void powerups_apply_to_ball(PowerupSystem* system, Ball* ball) {
    if (!system || !ball) return;
    
    for (int i = 0; i < system->active_count; i++) {
        ActivePowerup* powerup = &system->active_powerups[i];
        if (powerup->target != 2) continue; // 2 = bola
        
        switch (powerup->type) {
            case POWERUP_FIRE_BALL:
                break;
                
            case POWERUP_SPEED_BOOST:
                break;
                
            case POWERUP_GHOST_BALL:
                break;
                
            case POWERUP_ZIGZAG_BALL:
                powerup_effect_zigzag_ball(ball, 
                    powerup->data.zigzag.amplitude, 
                    powerup->data.zigzag.frequency);
                break;
                
            case POWERUP_CURVE_BALL:
                powerup_effect_curve_ball(ball, powerup->data.curve.strength);
                break;
                
            default:
                break;
        }
    }
}

void powerups_apply_to_paddle(PowerupSystem* system, Paddle* paddle, int side) {
    if (!system || !paddle) return;
    
    for (int i = 0; i < system->active_count; i++) {
        ActivePowerup* powerup = &system->active_powerups[i];
        if (powerup->target != side) continue; // 0 = jogador, 1 = inimigo
        
        switch (powerup->type) {
            case POWERUP_SIZE_BOOST:
                break;
                
            case POWERUP_SPEED_BOOST:
                break;
                
            case POWERUP_ENERGY_SHIELD:
                break;
                
            default:
                break;
        }
    }
}

void powerups_apply_to_environment(PowerupSystem* system) {
    if (!system) return;
    
    for (int i = 0; i < system->active_count; i++) {
        ActivePowerup* powerup = &system->active_powerups[i];
        if (powerup->target != 3) continue; // 3 = ambiente
        
        switch (powerup->type) {
            case POWERUP_SLOW_MOTION:
                break;
                
            case POWERUP_REVERSE_GRAVITY:
                break;
                
            case POWERUP_SCREEN_SHAKE:
                break;
                
            default:
                break;
        }
    }
}

float powerups_get_score_multiplier(PowerupSystem* system) {
    if (!system) return 1.0f;
    
    float multiplier = 1.0f;
    
    for (int i = 0; i < system->active_count; i++) {
        ActivePowerup* powerup = &system->active_powerups[i];
        
        if (powerup->type == POWERUP_SCORE_BOOST) {
            multiplier *= powerup->data.score_boost.score_multiplier;
        }
    }
    
    return multiplier;
}

void powerups_spawn_random(PowerupSystem* system, float x, float y) {
    if (!system || system->spawn_count >= MAX_POWERUP_SPAWNS) return;
    
    PowerupType types[] = {
        POWERUP_MULTI_BALL, POWERUP_SPEED_BOOST, POWERUP_SIZE_BOOST,
        POWERUP_FIRE_BALL, POWERUP_GHOST_BALL, POWERUP_CLONE_BALL,
        POWERUP_ZIGZAG_BALL, POWERUP_CURVE_BALL
    };
    
    int type_count = sizeof(types) / sizeof(types[0]);
    PowerupType selected_type = types[rand() % type_count];
    
    powerups_spawn_specific(system, selected_type, x, y);
}

void powerups_spawn_specific(PowerupSystem* system, PowerupType type, float x, float y) {
    if (!system || system->spawn_count >= MAX_POWERUP_SPAWNS) return;
    
    PowerupSpawn* spawn = &system->spawns[system->spawn_count];
    spawn->type = type;
    spawn->x = x;
    spawn->y = y;
    spawn->width = 32.0f;
    spawn->height = 32.0f;
    spawn->active = true;
    spawn->spawn_time = 0.0f;
    spawn->lifetime = 30.0f; // 30 segundos para coletar
    spawn->animation_time = 0.0f;
    spawn->rarity = powerups_get_rarity(type);
    spawn->collected = false;
    spawn->pulse_phase = 0.0f;
    
    system->spawn_count++;
}

void powerups_remove_spawn(PowerupSystem* system, int index) {
    if (!system || index < 0 || index >= system->spawn_count) return;
    
    system->spawns[index] = system->spawns[system->spawn_count - 1];
    system->spawn_count--;
}

void powerups_clear_spawns(PowerupSystem* system) {
    if (!system) return;
    system->spawn_count = 0;
}

int powerups_check_collection(PowerupSystem* system, float x, float y, float radius) {
    if (!system) return -1;
    
    for (int i = 0; i < system->spawn_count; i++) {
        PowerupSpawn* spawn = &system->spawns[i];
        if (!spawn->active || spawn->collected) continue;
        
        float dx = x - (spawn->x + spawn->width / 2);
        float dy = y - (spawn->y + spawn->height / 2);
        float distance = sqrtf(dx * dx + dy * dy);
        
        if (distance <= radius + spawn->width / 2) {
            spawn->collected = true;
            return i;
        }
    }
    
    return -1;
}

void powerups_set_spawn_rate(PowerupSystem* system, float rate) {
    if (system) system->spawn_rate_multiplier = rate;
}

void powerups_enable_spawning(PowerupSystem* system, bool enabled) {
    if (system) system->spawning_enabled = enabled;
}

void powerups_set_spawn_interval(PowerupSystem* system, float interval) {
    if (system) system->spawn_interval = interval;
}

void powerup_effect_triple_hit(Ball* ball, Paddle* paddle) {
}

void powerup_effect_turbo_smash(Ball* ball, float multiplier) {
}

void powerup_effect_energy_shield(Paddle* paddle, float duration) {
}

void powerup_effect_size_boost(Paddle* paddle, float multiplier, float duration) {
}

void powerup_effect_speed_boost(Paddle* paddle, float multiplier, float duration) {
}

void powerup_effect_magnetic_paddle(Paddle* paddle, Ball* ball, float radius) {
}

void powerup_effect_fire_ball(Ball* ball, float duration) {
}

void powerup_effect_ghost_ball(Ball* ball, float duration) {
}

void powerup_effect_multi_ball(PowerupSystem* system, Ball* original_ball, int count) {
}

void powerup_effect_slow_motion(float multiplier, float duration) {
}

void powerup_effect_precision_aim(Paddle* paddle, float precision, float duration) {
}

void powerup_effect_life_steal(Paddle* paddle, int amount) {
}

void powerup_effect_clone_ball(Ball* ball, int clone_count) {
    for (int i = 0; i < MAX_CLONE_BALLS && i < clone_count; i++) {
        if (!clone_balls[i].active) {
            clone_balls[i].active = true;
            clone_balls[i].x = ball->x + (rand() % 40 - 20);
            clone_balls[i].y = ball->y + (rand() % 40 - 20);
            clone_balls[i].vx = ball->vx + (rand() % 100 - 50);
            clone_balls[i].vy = ball->vy + (rand() % 100 - 50);
            clone_balls[i].lifetime = 10.0f;
        }
    }
}

void powerup_effect_zigzag_ball(Ball* ball, float amplitude, float frequency) {
    static float time = 0.0f;
    time += 1.0f / 60.0f; // Assumindo 60 FPS
    
    float offset = amplitude * sinf(frequency * time);
}

void powerup_effect_curve_ball(Ball* ball, float strength) {
    float curve_x = -ball->vy * strength;
    float curve_y = ball->vx * strength;
    
    ball->vx += curve_x;
    ball->vy += curve_y;
}

void powerup_effect_spiral_ball(Ball* ball, float radius) {
    static float angle = 0.0f;
    angle += 0.1f;
    
    float spiral_x = radius * cosf(angle);
    float spiral_y = radius * sinf(angle);
    
    ball->x += spiral_x;
    ball->y += spiral_y;
}

void powerup_effect_teleport_ball(Ball* ball, float interval) {
    static float timer = 0.0f;
    timer += 1.0f / 60.0f;
    
    if (timer >= interval) {
        timer = 0.0f;
        ball->x = 200 + (rand() % 400);
        ball->y = 100 + (rand() % 400);
    }
}

void powerup_effect_invisible_ball(Ball* ball) {
}

void powerup_effect_phase_ball(Ball* ball) {
}

void powerup_effect_homing_ball(Ball* ball) {
}

PowerupRarity powerups_get_rarity(PowerupType type) {
    switch (type) {
        case POWERUP_MULTI_BALL:
        case POWERUP_EXPLOSIVE_BALL:
        case POWERUP_TELEPORT_BALL:
            return RARITY_LEGENDARY;
            
        case POWERUP_FIRE_BALL:
        case POWERUP_GHOST_BALL:
        case POWERUP_CLONE_BALL:
            return RARITY_EPIC;
            
        case POWERUP_SPEED_BOOST:
        case POWERUP_SIZE_BOOST:
        case POWERUP_CURVE_BALL:
            return RARITY_RARE;
            
        default:
            return RARITY_COMMON;
    }
}

float powerups_get_base_duration(PowerupType type) {
    if (type < sizeof(powerup_base_durations) / sizeof(powerup_base_durations[0])) {
        return powerup_base_durations[type];
    }
    return 10.0f; // Duração padrão
}

const char* powerups_get_name(PowerupType type) {
    switch (type) {
        case POWERUP_MULTI_BALL: return "Multi Ball";
        case POWERUP_TRIPLE_HIT: return "Triple Hit";
        case POWERUP_TURBO_SMASH: return "Turbo Smash";
        case POWERUP_ENERGY_SHIELD: return "Energy Shield";
        case POWERUP_SIZE_BOOST: return "Size Boost";
        case POWERUP_SPEED_BOOST: return "Speed Boost";
        case POWERUP_MAGNETIC_PADDLE: return "Magnetic Paddle";
        case POWERUP_FIRE_BALL: return "Fire Ball";
        case POWERUP_GHOST_BALL: return "Ghost Ball";
        case POWERUP_SLOW_MOTION: return "Slow Motion";
        case POWERUP_PRECISION_AIM: return "Precision Aim";
        case POWERUP_LIFE_STEAL: return "Life Steal";
        case POWERUP_CLONE_BALL: return "Clone Ball";
        case POWERUP_INVISIBLE_BALL: return "Invisible Ball";
        case POWERUP_ZIGZAG_BALL: return "Zigzag Ball";
        case POWERUP_GRAVITY_BALL: return "Gravity Ball";
        case POWERUP_TELEPORT_BALL: return "Teleport Ball";
        case POWERUP_PHASE_BALL: return "Phase Ball";
        case POWERUP_HOMING_BALL: return "Homing Ball";
        case POWERUP_CURVE_BALL: return "Curve Ball";
        case POWERUP_SPIRAL_BALL: return "Spiral Ball";
        case POWERUP_EXPLOSIVE_BALL: return "Explosive Ball";
        default: return "Unknown";
    }
}

const char* powerups_get_description(PowerupType type) {
    switch (type) {
        case POWERUP_MULTI_BALL: return "Cria múltiplas bolas";
        case POWERUP_FIRE_BALL: return "Bola em chamas causa dano extra";
        case POWERUP_GHOST_BALL: return "Bola atravessa obstáculos";
        case POWERUP_CLONE_BALL: return "Cria clones da bola";
        case POWERUP_ZIGZAG_BALL: return "Bola faz movimento zigue-zague";
        case POWERUP_CURVE_BALL: return "Bola faz curvas suaves";
        default: return "Power-up especial";
    }
}

bool powerups_can_stack(PowerupType type) {
    switch (type) {
        case POWERUP_MULTI_BALL:
        case POWERUP_CLONE_BALL:
        case POWERUP_LIFE_STEAL:
            return true;
        default:
            return false;
    }
}

int powerups_get_cost(PowerupType type) {
    PowerupRarity rarity = powerups_get_rarity(type);
    switch (rarity) {
        case RARITY_COMMON: return 100;
        case RARITY_RARE: return 250;
        case RARITY_EPIC: return 500;
        case RARITY_LEGENDARY: return 1000;
        default: return 100;
    }
}

void powerups_spawn_particle_effect(float x, float y, PowerupType type) {
}

void powerups_show_collection_effect(float x, float y, PowerupType type) {
}

void powerups_update_spawn_animation(PowerupSpawn* spawn, float delta_time) {
    if (!spawn || !spawn->active) return;
    
    spawn->animation_time += delta_time;
    spawn->pulse_phase += delta_time * 3.0f; // 3 Hz pulse
    
    float float_offset = 5.0f * sinf(spawn->animation_time * 2.0f);
    spawn->y += float_offset * delta_time;
}

