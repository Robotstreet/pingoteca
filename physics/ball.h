#ifndef BALL_H
#define BALL_H

#include <stdbool.h>

typedef struct {
    float x, y;           // Posição
    float vx, vy;         // Velocidade
    float radius;         // Raio
    float speed;          // Velocidade base
    float max_speed;      // Velocidade máxima
    bool active;          // Se a bola está ativa
    int trail_count;      // Contador para efeito de rastro
    float spin;           // Efeito da bola
    int bounce_count;     // Contador de quiques
    bool fire_mode;       // Modo fogo ativo
    float fire_timer;     // Timer do modo fogo

    
    bool is_clone;            // Se é um clone da bola
    bool is_real_ball;        // Se é a bola real (para sistema de clones)
    bool invisible;           // Se está invisível
    bool phasing;            // Se atravessa obstáculos
    bool homing;             // Se persegue raquete inimiga
    
    bool zigzag_mode;        // Movimento zigue-zague
    float zigzag_amplitude;  // Amplitude do zigue-zague
    float zigzag_frequency;  // Frequência do zigue-zague
    float zigzag_timer;      // Timer para o movimento
    
    bool curve_mode;         // Movimento em curva
    float curve_strength;    // Força da curva
    
    bool spiral_mode;        // Movimento espiral
    float spiral_radius;     // Raio do espiral
    float spiral_angle;      // Ângulo atual do espiral
    
    bool gravity_affected;   // Se é afetada pela gravidade especial
    float gravity_multiplier;// Multiplicador de gravidade
    
    bool teleport_mode;      // Se pode teleportar
    float teleport_timer;    // Timer para próximo teleporte
    float teleport_interval; // Intervalo entre teleportes
    
    bool heavy;              // Bola pesada
    bool light;              // Bola leve
    bool magnetic;           // Atraída por raquetes
    bool electric;           // Efeito elétrico
    bool ice;                // Efeito de gelo
    bool poison;             // Efeito venenoso
    bool explosive;          // Explode ao tocar
    
    float effect_timer;      // Timer para efeitos temporários
	float gravity_scale;      // Escala de afetação pela gravidade
	bool affected_by_wind;    // Se é afetada pelo vento
    int bounce_multiplier;   // Multiplicador de quiques
} Ball;

typedef enum {
    COLLISION_NONE = 0,
    COLLISION_PADDLE,
    COLLISION_WALL,
    COLLISION_WALL_TOP,
    COLLISION_WALL_BOTTOM,
    COLLISION_WALL_LEFT,
    COLLISION_WALL_RIGHT,
    COLLISION_OBSTACLE
} CollisionType;




void ball_init(Ball* ball, float x, float y);
void ball_reset(Ball* ball, float x, float y);
void ball_update(Ball* ball, float delta_time);
void ball_apply_velocity(Ball* ball, float vx, float vy);
void ball_apply_spin(Ball* ball, float spin_amount);
void ball_increase_speed(Ball* ball, float multiplier);
void ball_activate_fire_mode(Ball* ball, float duration);
bool ball_is_fire_mode_active(Ball* ball);
void ball_reverse_x(Ball* ball);
void ball_reverse_y(Ball* ball);
float ball_get_speed(Ball* ball);
void ball_set_position(Ball* ball, float x, float y);
void ball_add_trail_effect(Ball* ball);

void ball_activate_zigzag(Ball* ball, float amplitude, float frequency, float duration);
void ball_activate_curve(Ball* ball, float strength, float duration);
void ball_activate_spiral(Ball* ball, float radius, float duration);
void ball_activate_homing(Ball* ball, float duration);
void ball_activate_teleport(Ball* ball, float interval, float duration);
void ball_make_invisible(Ball* ball, float duration);
void ball_enable_phasing(Ball* ball, float duration);
void ball_apply_special_physics(Ball* ball, float delta_time);
Ball* ball_create_clone(Ball* original, bool is_real);

float ball_get_effective_radius(Ball* ball);
void ball_clamp_speed(Ball* ball);

#endif // BALL_H
