#ifndef CHEAT_SYSTEM_H
#define CHEAT_SYSTEM_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#define MAX_CHEAT_INPUT 32
#define MAX_CHEAT_NAME 64
#define MAX_CONFIRMATION_BOXES 5

typedef enum {
    CHEAT_CONTEXT_GAMEPLAY,
    CHEAT_CONTEXT_MENU,
    CHEAT_CONTEXT_LEVEL_SELECT,
    CHEAT_CONTEXT_OPTIONS
} CheatContext;

typedef struct {
    bool active;
    char text[128];
    float timer;
    float animation_offset; // Para animação de entrada
    float target_y; // Posição Y alvo
} ConfirmationBox;

typedef struct {
    bool active;
    bool interface_open;
    char input_buffer[MAX_CHEAT_INPUT];
    int input_length;
    
    ConfirmationBox confirmation_boxes[MAX_CONFIRMATION_BOXES];
    int active_boxes_count;
    
    CheatContext current_context;
    
    bool all_levels_unlocked;
    bool ai_frozen;
    bool godmode_paddle;
    bool super_ball;
    bool noclip_mode;
    bool infinite_power;
    bool rainbow_mode;
    bool slow_motion;
    bool giant_paddle;
    bool instant_win;
    bool ball_control; 
    
} CheatSystem;

void cheat_system_init(CheatSystem* cheat);
void cheat_system_update(CheatSystem* cheat, float delta_time);
void cheat_system_render(CheatSystem* cheat, SDL_Renderer* renderer);
void cheat_system_handle_input(CheatSystem* cheat, SDL_Event* event);
void cheat_system_toggle_interface(CheatSystem* cheat);
bool cheat_system_is_interface_open(CheatSystem* cheat);
void cheat_system_set_context(CheatSystem* cheat, CheatContext context);

#endif
