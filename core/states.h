#ifndef STATES_H
#define STATES_H

#include "config.h"
#include <stdbool.h>

typedef struct StateManager StateManager;
typedef struct State State;

typedef void (*StateInitFunc)(void);
typedef void (*StateUpdateFunc)(float delta_time);
typedef void (*StateRenderFunc)(void);
typedef void (*StateCleanupFunc)(void);
typedef void (*StateEnterFunc)(void);
typedef void (*StateExitFunc)(void);

typedef struct State {
    GameState id;
    StateInitFunc init;
    StateUpdateFunc update;
    StateRenderFunc render;
    StateCleanupFunc cleanup;
    StateEnterFunc enter;
    StateExitFunc exit;
    bool initialized;
    bool active;
    void* data; // Dados específicos do estado
} State;

typedef struct StateManager {
    State states[STATE_EXIT + 1];
    GameState current_state;
    GameState next_state;
    bool transition_pending;
    float transition_time;
    float transition_duration;
    bool transitioning;
} StateManager;

extern StateManager g_state_manager;

void states_init(void);
void states_update(float delta_time);
void states_render(void);
void states_cleanup(void);

void state_change(GameState new_state);
void state_change_with_transition(GameState new_state, float duration);
GameState state_get_current(void);
bool state_is_transitioning(void);

void state_register(GameState id,
                   StateInitFunc init,
                   StateUpdateFunc update,
                   StateRenderFunc render,
                   StateCleanupFunc cleanup,
                   StateEnterFunc enter,
                   StateExitFunc exit);

void splash_state_init(void);
void splash_state_update(float delta_time);
void splash_state_render(void);
void splash_state_cleanup(void);
void splash_state_enter(void);
void splash_state_exit(void);

void menu_state_init(void);
void menu_state_update(float delta_time);
void menu_state_render(void);
void menu_state_cleanup(void);
void menu_state_enter(void);
void menu_state_exit(void);

void level_select_state_init(void);
void level_select_state_update(float delta_time);
void level_select_state_render(void);
void level_select_state_cleanup(void);
void level_select_state_enter(void);
void level_select_state_exit(void);

void gameplay_state_init(void);
void gameplay_state_update(float delta_time);
void gameplay_state_render(void);
void gameplay_state_cleanup(void);
void gameplay_state_enter(void);
void gameplay_state_exit(void);

void pause_state_init(void);
void pause_state_update(float delta_time);
void pause_state_render(void);
void pause_state_cleanup(void);
void pause_state_enter(void);
void pause_state_exit(void);

void game_over_state_init(void);
void game_over_state_update(float delta_time);
void game_over_state_render(void);
void game_over_state_cleanup(void);
void game_over_state_enter(void);
void game_over_state_exit(void);

void victory_state_init(void);
void victory_state_update(float delta_time);
void victory_state_render(void);
void victory_state_cleanup(void);
void victory_state_enter(void);
void victory_state_exit(void);

#endif // STATES_H
