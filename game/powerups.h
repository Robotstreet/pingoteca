#ifndef POWERUPS_H
#define POWERUPS_H

#include <stdbool.h>
#include "../physics/ball.h"
#include "../physics/paddle.h"

#define MAX_ACTIVE_POWERUPS 16
#define MAX_POWERUP_SPAWNS 8

typedef enum {
    RARITY_COMMON = 1,
    RARITY_RARE = 2,
    RARITY_EPIC = 3,
    RARITY_LEGENDARY = 4
} PowerupRarity;

typedef enum {
    POWERUP_MULTI_BALL = 0,
    POWERUP_TRIPLE_HIT,
    POWERUP_TURBO_SMASH,
    POWERUP_ENERGY_SHIELD,
    POWERUP_SIZE_BOOST,
    POWERUP_SPEED_BOOST,
    POWERUP_MAGNETIC_PADDLE,
    POWERUP_FIRE_BALL,
    POWERUP_GHOST_BALL,
    POWERUP_SLOW_MOTION,
    POWERUP_PRECISION_AIM,
    POWERUP_LIFE_STEAL,
    
    POWERUP_CLONE_BALL,        // Cria clones da bola (só um é real)
    POWERUP_INVISIBLE_BALL,    // Bola fica invisível
    POWERUP_ZIGZAG_BALL,       // Bola faz zigue-zague
    POWERUP_GRAVITY_BALL,      // Bola curva pela gravidade
    POWERUP_TELEPORT_BALL,     // Bola teleporta aleatoriamente
    POWERUP_PHASE_BALL,        // Bola atravessa obstáculos
    POWERUP_HOMING_BALL,       // Bola persegue raquete inimiga
    POWERUP_SPLIT_BALL,        // Bola se divide ao tocar raquete
    POWERUP_CURVE_BALL,        // Bola faz curva suave
    POWERUP_SPIRAL_BALL,       // Movimento espiral
    POWERUP_BOUNCY_BALL,       // Quica mais vezes
    POWERUP_HEAVY_BALL,        // Pesada, quebra obstáculos
    POWERUP_LIGHT_BALL,        // Leve, afetada pelo vento
    POWERUP_MAGNETIC_BALL,     // Atraída por raquetes
    POWERUP_ELECTRIC_BALL,     // Atordoa ao tocar
    POWERUP_ICE_BALL,          // Congela ao tocar
    POWERUP_POISON_BALL,       // Envenena
    POWERUP_EXPLOSIVE_BALL,    // Explode
    
    POWERUP_ENEMY_SLOW,        // Inimigo fica lento
    POWERUP_ENEMY_BLIND,       // Cegueira no inimigo
    POWERUP_ENEMY_CONFUSION,   // Inverte controles
    POWERUP_ENEMY_FREEZE,      // Congela inimigo
    POWERUP_ENEMY_SHRINK,      // Diminui raquete inimiga
    POWERUP_ENEMY_DIZZY,       // Inimigo fica tonto
    
    POWERUP_PLAYER_SLOW,       // Jogador fica lento
    POWERUP_PLAYER_BLIND,      // Cegueira no jogador
    POWERUP_PLAYER_CONFUSION,  // Inverte controles
    POWERUP_PLAYER_SHRINK,     // Diminui raquete
    POWERUP_PLAYER_DIZZY,      // Jogador fica tonto
    
    POWERUP_FOG_SCREEN,        // Neblina na tela
    POWERUP_REVERSE_GRAVITY,   // Inverte gravidade
    POWERUP_SCREEN_SHAKE,      // Treme a tela
    POWERUP_COLOR_BLIND,       // Remove cores
    POWERUP_DISCO_LIGHTS,      // Luzes piscando
    POWERUP_MIRROR_WORLD,      // Espelha mundo
    POWERUP_UPSIDE_DOWN,       // Inverte tela
    POWERUP_STROBE_EFFECT,     // Efeito estroboscópico
    POWERUP_RAIN_EFFECT,       // Chuva na tela
    POWERUP_SNOW_EFFECT,       // Neve na tela
    
    POWERUP_SCORE_BOOST
} PowerupType;

typedef struct {
    PowerupType type;
    float duration;
    float remaining_time;
    float intensity;
    bool persistent;      // Se persiste entre fases
    int target;          // 0 = jogador, 1 = inimigo, 2 = bola, 3 = ambiente

    union {
        struct { int extra_balls; } multi_ball;
        struct { float speed_multiplier; } speed_boost;
        struct { float size_multiplier; } size_boost;
        struct { float score_multiplier; } score_boost;
        struct { float magnetic_radius; } magnetic;
        struct { int damage_amount; } damage;
        struct { float heal_amount; } healing;
        struct { 
            int clone_count; 
            float clone_offset; 
        } clone_ball;
        struct { 
            float amplitude; 
            float frequency; 
        } zigzag;
        struct { 
            float strength; 
        } curve;
        struct { 
            float radius; 
        } spiral;
        struct { 
            float interval; 
        } teleport;
    } data;
} ActivePowerup;

typedef struct {
    PowerupType type;
    float x, y;
    float width, height;
    bool active;
    float spawn_time;
    float lifetime;
    float animation_time;
    int rarity;           // 1 = comum, 2 = raro, 3 = épico, 4 = lendário
    bool collected;
    float pulse_phase;    // Para animação de pulso
} PowerupSpawn;

typedef struct {
    ActivePowerup active_powerups[MAX_ACTIVE_POWERUPS];
    int active_count;

    PowerupSpawn spawns[MAX_POWERUP_SPAWNS];
    int spawn_count;

    float spawn_timer;
    float spawn_interval;
    bool spawning_enabled;

    float duration_multiplier;
    float spawn_rate_multiplier;
    int max_simultaneous;
    bool allow_stacking;
} PowerupSystem;

void powerups_init(PowerupSystem* system);
void powerups_cleanup(PowerupSystem* system);
void powerups_update(PowerupSystem* system, float delta_time);
void powerups_render_effects(PowerupSystem* system);

void powerups_activate(PowerupSystem* system, PowerupType type, float duration, int target);
void powerups_deactivate(PowerupSystem* system, PowerupType type, int target);
void powerups_clear_all(PowerupSystem* system);
bool powerups_is_active(PowerupSystem* system, PowerupType type, int target);
float powerups_get_remaining_time(PowerupSystem* system, PowerupType type, int target);
int powerups_get_active_count(PowerupSystem* system);

void powerups_apply_to_ball(PowerupSystem* system, Ball* ball);
void powerups_apply_to_paddle(PowerupSystem* system, Paddle* paddle, int side);
void powerups_apply_to_environment(PowerupSystem* system);
float powerups_get_score_multiplier(PowerupSystem* system);

void powerups_spawn_random(PowerupSystem* system, float x, float y);
void powerups_spawn_specific(PowerupSystem* system, PowerupType type, float x, float y);
void powerups_remove_spawn(PowerupSystem* system, int index);
void powerups_clear_spawns(PowerupSystem* system);
int powerups_check_collection(PowerupSystem* system, float x, float y, float radius);

void powerups_set_spawn_rate(PowerupSystem* system, float rate);
void powerups_enable_spawning(PowerupSystem* system, bool enabled);
void powerups_set_spawn_interval(PowerupSystem* system, float interval);

void powerup_effect_triple_hit(Ball* ball, Paddle* paddle);
void powerup_effect_turbo_smash(Ball* ball, float multiplier);
void powerup_effect_energy_shield(Paddle* paddle, float duration);
void powerup_effect_size_boost(Paddle* paddle, float multiplier, float duration);
void powerup_effect_speed_boost(Paddle* paddle, float multiplier, float duration);
void powerup_effect_magnetic_paddle(Paddle* paddle, Ball* ball, float radius);
void powerup_effect_fire_ball(Ball* ball, float duration);
void powerup_effect_ghost_ball(Ball* ball, float duration);
void powerup_effect_multi_ball(PowerupSystem* system, Ball* original_ball, int count);
void powerup_effect_slow_motion(float multiplier, float duration);
void powerup_effect_precision_aim(Paddle* paddle, float precision, float duration);
void powerup_effect_life_steal(Paddle* paddle, int amount);

void powerup_effect_clone_ball(Ball* ball, int clone_count);
void powerup_effect_zigzag_ball(Ball* ball, float amplitude, float frequency);
void powerup_effect_curve_ball(Ball* ball, float strength);
void powerup_effect_spiral_ball(Ball* ball, float radius);
void powerup_effect_teleport_ball(Ball* ball, float interval);
void powerup_effect_invisible_ball(Ball* ball);
void powerup_effect_phase_ball(Ball* ball);
void powerup_effect_homing_ball(Ball* ball);

PowerupRarity powerups_get_rarity(PowerupType type);
float powerups_get_base_duration(PowerupType type);
const char* powerups_get_name(PowerupType type);
const char* powerups_get_description(PowerupType type);
bool powerups_can_stack(PowerupType type);
int powerups_get_cost(PowerupType type);

void powerups_spawn_particle_effect(float x, float y, PowerupType type);
void powerups_show_collection_effect(float x, float y, PowerupType type);
void powerups_update_spawn_animation(PowerupSpawn* spawn, float delta_time);

#endif // POWERUPS_H
