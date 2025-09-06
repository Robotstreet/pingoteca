#ifndef ENEMY_H
#define ENEMY_H

#include <stdbool.h>
#include "../physics/paddle.h"
#include "../physics/ball.h"
#include "powerups.h" 

#define MAX_ENEMY_NAME 32

typedef enum {
    AI_STATE_TRACKING = 0,
    AI_STATE_ATTACKING,
    AI_STATE_DEFENDING,
    AI_STATE_SPECIAL_MOVE,
    AI_STATE_RECOVERING,
    AI_STATE_STUNNED
} AIState;


typedef enum {
    POWER_NONE,
    POWER_ERRO_BOBO,           // Iniciante
    POWER_MURALHA_ACO,         // Defensor  
    POWER_BOLADA_ATORDOANTE,   // Agressor
    POWER_TROCA_PERCURSO,      // Adaptativo
    POWER_NEBLINA_DENSA,       // Boss Tempestade
    POWER_CHOQUE_PARALISANTE,  // Boss Relâmpago
    POWER_BOLA_CAOTICA,        // Boss Caos
    POWER_CLONE_SOMBRAS,       // Boss Clonador
    POWER_DOMINIO_ABSOLUTO     // Boss Mestre
} EnemyPowerType;

typedef enum {
    ENEMY_BEGINNER = 0,    // Iniciante
    ENEMY_DEFENDER = 1,    // Defensor  
    ENEMY_AGGRESSOR = 2,   // Agressor
    ENEMY_ADAPTIVE = 3,    // Adaptativo
    ENEMY_BOSS_STORM = 4,  // Boss Tempestade
    ENEMY_BOSS_LIGHTNING = 5, // Boss Relâmpago
    ENEMY_BOSS_CHAOS = 6,  // Boss Caos
    ENEMY_BOSS_MASTER = 7  // Boss Mestre (Capitão VM)
} EnemyType;

typedef struct {
    bool active;
    float timer;
    float duration;
    float intensity;
    int effect_type;
    float pos_x, pos_y;
    float color_r, color_g, color_b, color_a;
} PowerEffect;

typedef struct {
    bool active;
    float x, y;
    float width, height;
    float timer;
    float alpha;
} Barrier;

typedef struct {
    bool active;
    float x, y;
    float width, height;   
    float target_y;
    float move_speed;
    float timer;
    float alpha;
} ShadowClone;

typedef struct {
    bool active;
    float timer;
    float duration;
    float intensity;
    int effect_type;
    float pos_x, pos_y;
    float color_r, color_g, color_b, color_a;
    float radius;
    float oscillation_speed;
} PrePowerEffect;

typedef struct {
    bool active;
    float timer;
    float color_alternation_timer;
    bool is_white_phase;
    float start_x, start_y;
    float end_x, end_y;
} ElectricRayEffect;

typedef struct {
    char name[MAX_ENEMY_NAME];
    EnemyType type;
    int level;
    int health;
    int max_health;
    float difficulty_multiplier;

    AIState ai_state;
    float ai_timer;
    float reaction_time;
    float prediction_accuracy;
    float aggression;
    float defense_bias;
    float error_rate;

    float move_speed_base;
    float move_speed_current;
    float target_y;
    float center_bias;    // Tendência de voltar ao centro
    float edge_avoidance; // Evitar bordas

    bool can_use_powerups;
    float powerup_cooldown;
    float special_move_cooldown;
    bool in_special_move;
    float special_duration;

    int movement_pattern;
    float pattern_timer;
    float pattern_amplitude;
    float pattern_frequency;

    int hits_made;
    int hits_received;
    int special_moves_used;
    float damage_dealt;
    float accuracy;

    bool is_stunned;
    float stun_duration;
    bool is_enraged;
    float rage_multiplier;

    Paddle paddle;
    
    EnemyPowerType power_type;
    float power_cooldown;
    float power_duration;
    bool power_active;
    float power_timer;
    
    PowerEffect visual_effect;
    
    Barrier barrier;           // Muralha de Aço
    ShadowClone clone;         // Clone das Sombras
    bool player_stunned;       // Atordoamento
    float stun_timer;
    
    float fog_intensity;
    
    Ball fake_balls[2];
    bool fake_balls_active;
    
    EnemyPowerType current_master_power;
    float master_power_switch_timer;
    
	PrePowerEffect pre_effect;           // Efeito visual antes do poder
	ElectricRayEffect electric_ray;      // Efeito de raio elétrico
	bool power_charging;                 // Indicador se está carregando poder
	float power_charge_timer;            // Timer para carregar poder
    
} Enemy;

typedef enum {
    PATTERN_STRAIGHT = 0,
    PATTERN_SINE_WAVE,
    PATTERN_TRIANGLE_WAVE,
    PATTERN_CIRCULAR,
    PATTERN_ZIGZAG,
    PATTERN_RANDOM,
    PATTERN_ADAPTIVE,
    PATTERN_MIRROR_PLAYER
} MovementPattern;



void enemy_init(Enemy* enemy, EnemyType type, int level);
void enemy_update(Enemy* enemy, Ball* ball, Paddle* player_paddle, float delta_time);
void enemy_reset(Enemy* enemy);
void enemy_take_damage(Enemy* enemy, int damage);
void enemy_heal(Enemy* enemy, int health);
bool enemy_is_alive(Enemy* enemy);

void enemy_update_ai(Enemy* enemy, Ball* ball, Paddle* player_paddle, float delta_time);
void enemy_calculate_target(Enemy* enemy, Ball* ball, Paddle* player_paddle);
void enemy_move_to_target(Enemy* enemy, float delta_time);
void enemy_predict_ball_position(Enemy* enemy, Ball* ball, float* predicted_y);
void enemy_adjust_difficulty(Enemy* enemy, float player_performance);

void enemy_set_ai_state(Enemy* enemy, AIState state);
void enemy_update_tracking(Enemy* enemy, Ball* ball, float delta_time);
void enemy_update_attacking(Enemy* enemy, Ball* ball, float delta_time);
void enemy_update_defending(Enemy* enemy, Ball* ball, float delta_time);
void enemy_update_special_move(Enemy* enemy, Ball* ball, float delta_time);

bool enemy_can_use_special(Enemy* enemy);
void enemy_use_special_move(Enemy* enemy, Ball* ball);
void enemy_activate_powerup(Enemy* enemy, PowerupType type);
void enemy_apply_boss_ability(Enemy* enemy, Ball* ball);

void enemy_set_movement_pattern(Enemy* enemy, MovementPattern pattern);
void enemy_update_movement_pattern(Enemy* enemy, float delta_time);
float enemy_calculate_pattern_offset(Enemy* enemy, MovementPattern pattern);

void enemy_stun(Enemy* enemy, float duration);
void enemy_enrage(Enemy* enemy, float multiplier, float duration);
void enemy_slow(Enemy* enemy, float multiplier, float duration);
void enemy_confuse(Enemy* enemy, float duration);

void enemy_increase_difficulty(Enemy* enemy, float amount);
void enemy_decrease_difficulty(Enemy* enemy, float amount);
float enemy_get_difficulty_rating(Enemy* enemy);
void enemy_balance_difficulty(Enemy* enemy, float target_win_rate);

float enemy_get_distance_to_ball(Enemy* enemy, Ball* ball);
bool enemy_ball_coming_to_side(Enemy* enemy, Ball* ball);
float enemy_calculate_interception_time(Enemy* enemy, Ball* ball);
void enemy_apply_error(Enemy* enemy, float* target_y);

#endif // ENEMY_H
