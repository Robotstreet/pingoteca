#include "states.h"
#include "engine.h"
#include <stdio.h>
#include <string.h>

StateManager g_state_manager = {0};

void states_init(void) {
    printf("Inicializando sistema de estados...\n");

    memset(&g_state_manager, 0, sizeof(StateManager));
    g_state_manager.current_state = STATE_SPLASH;
    g_state_manager.next_state = STATE_SPLASH;
    g_state_manager.transition_pending = false;
    g_state_manager.transitioning = false;
    g_state_manager.transition_time = 0.0f;
    g_state_manager.transition_duration = 0.5f;

    state_register(STATE_SPLASH,
                   splash_state_init,
                   splash_state_update,
                   splash_state_render,
                   splash_state_cleanup,
                   splash_state_enter,
                   splash_state_exit);

    state_register(STATE_MAIN_MENU,
                   menu_state_init,
                   menu_state_update,
                   menu_state_render,
                   menu_state_cleanup,
                   menu_state_enter,
                   menu_state_exit);

    state_register(STATE_LEVEL_SELECT,
                   level_select_state_init,
                   level_select_state_update,
                   level_select_state_render,
                   level_select_state_cleanup,
                   level_select_state_enter,
                   level_select_state_exit);

    state_register(STATE_GAMEPLAY,
                   gameplay_state_init,
                   gameplay_state_update,
                   gameplay_state_render,
                   gameplay_state_cleanup,
                   gameplay_state_enter,
                   gameplay_state_exit);

    state_register(STATE_PAUSE,
                   pause_state_init,
                   pause_state_update,
                   pause_state_render,
                   pause_state_cleanup,
                   pause_state_enter,
                   pause_state_exit);

    state_register(STATE_GAME_OVER,
                   game_over_state_init,
                   game_over_state_update,
                   game_over_state_render,
                   game_over_state_cleanup,
                   game_over_state_enter,
                   game_over_state_exit);

    state_register(STATE_VICTORY,
                   victory_state_init,
                   victory_state_update,
                   victory_state_render,
                   victory_state_cleanup,
                   victory_state_enter,
                   victory_state_exit);

    State* initial_state = &g_state_manager.states[g_state_manager.current_state];
    if (initial_state->init) {
        initial_state->init();
        initial_state->initialized = true;
    }
    if (initial_state->enter) {
        initial_state->enter();
    }
    initial_state->active = true;

    printf("Sistema de estados inicializado.\n");
}

void states_update(float delta_time) {
    if (g_state_manager.transition_pending) {
        if (!g_state_manager.transitioning) {
            State* current_state = &g_state_manager.states[g_state_manager.current_state];
            if (current_state->exit) {
                current_state->exit();
            }
            current_state->active = false;

            g_state_manager.transitioning = true;
            g_state_manager.transition_time = 0.0f;
        }

        g_state_manager.transition_time += delta_time;

        if (g_state_manager.transition_time >= g_state_manager.transition_duration) {
            g_state_manager.current_state = g_state_manager.next_state;
            State* new_state = &g_state_manager.states[g_state_manager.current_state];

            if (!new_state->initialized && new_state->init) {
                new_state->init();
                new_state->initialized = true;
            }

            if (new_state->enter) {
                new_state->enter();
            }
            new_state->active = true;

            g_state_manager.transition_pending = false;
            g_state_manager.transitioning = false;
        }
    }

    if (!g_state_manager.transitioning) {
        State* current_state = &g_state_manager.states[g_state_manager.current_state];
        if (current_state->active && current_state->update) {
            current_state->update(delta_time);
        }
    }
}

void states_render(void) {
    State* current_state = &g_state_manager.states[g_state_manager.current_state];
    if (current_state->active && current_state->render) {
        current_state->render();
    }

    if (g_state_manager.transitioning) {
        float progress = g_state_manager.transition_time / g_state_manager.transition_duration;
        int alpha = (int)(255 * progress);

        SDL_Renderer* renderer = engine_get_renderer();
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);

        SDL_Rect screen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &screen);
    }
}

void states_cleanup(void) {
    printf("Finalizando sistema de estados...\n");

    for (int i = 0; i <= STATE_EXIT; i++) {
        State* state = &g_state_manager.states[i];
        if (state->initialized && state->cleanup) {
            state->cleanup();
        }
    }

    printf("Sistema de estados finalizado.\n");
}

void state_change(GameState new_state) {
    if (new_state == g_state_manager.current_state) {
        return;
    }

    printf("Mudando estado: %d -> %d\n", g_state_manager.current_state, new_state);

    g_state_manager.next_state = new_state;
    g_state_manager.transition_pending = true;
    g_state_manager.transition_duration = 0.1f; // Transição rápida
}

void state_change_with_transition(GameState new_state, float duration) {
    if (new_state == g_state_manager.current_state) {
        return;
    }

    printf("Mudando estado com transição: %d -> %d (%.2fs)\n",
           g_state_manager.current_state, new_state, duration);

    g_state_manager.next_state = new_state;
    g_state_manager.transition_pending = true;
    g_state_manager.transition_duration = duration;
}

GameState state_get_current(void) {
    return g_state_manager.current_state;
}

bool state_is_transitioning(void) {
    return g_state_manager.transitioning;
}

void state_register(GameState id,
                   StateInitFunc init,
                   StateUpdateFunc update,
                   StateRenderFunc render,
                   StateCleanupFunc cleanup,
                   StateEnterFunc enter,
                   StateExitFunc exit) {

    State* state = &g_state_manager.states[id];
    state->id = id;
    state->init = init;
    state->update = update;
    state->render = render;
    state->cleanup = cleanup;
    state->enter = enter;
    state->exit = exit;
    state->initialized = false;
    state->active = false;
    state->data = NULL;
}


static float splash_timer = 0.0f;
static const float SPLASH_DURATION = 3.0f;

void splash_state_init(void) {
    printf("Inicializando estado splash...\n");
    splash_timer = 0.0f;
}

void splash_state_update(float delta_time) {
    splash_timer += delta_time;

    if (splash_timer >= SPLASH_DURATION) {
        state_change_with_transition(STATE_MAIN_MENU, 0.5f);
    }
}

void splash_state_render(void) {
    SDL_Renderer* renderer = engine_get_renderer();

    SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
    SDL_RenderClear(renderer);

    float progress = splash_timer / SPLASH_DURATION;
    int alpha = (int)(255 * (progress < 0.5f ? progress * 2 : (1.0f - progress) * 2));

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 64, 128, 255, alpha);

    SDL_Rect logo = {
        SCREEN_WIDTH / 2 - 100,
        SCREEN_HEIGHT / 2 - 50,
        200, 100
    };
    SDL_RenderFillRect(renderer, &logo);
}

void splash_state_cleanup(void) {
    printf("Finalizando estado splash.\n");
}

void splash_state_enter(void) {
    splash_timer = 0.0f;
}

void splash_state_exit(void) {
}

static int menu_selection = 0;
static const int MENU_ITEMS = 4;
static const char* menu_options[] = {"Jogar", "Tutorial", "Rankings", "Sair"};

void menu_state_init(void) {
    printf("Inicializando menu principal...\n");
    menu_selection = 0;
}

void menu_state_update(float delta_time) {
    static float auto_timer = 0.0f;
    auto_timer += delta_time;

    if (auto_timer >= 5.0f) {
        state_change_with_transition(STATE_LEVEL_SELECT, 0.3f);
        auto_timer = 0.0f;
    }
}

void menu_state_render(void) {
    SDL_Renderer* renderer = engine_get_renderer();

    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < MENU_ITEMS; i++) {
        SDL_Rect item = {
            SCREEN_WIDTH / 2 - 100,
            SCREEN_HEIGHT / 2 - 80 + i * 40,
            200, 30
        };

        if (i == menu_selection) {
            SDL_SetRenderDrawColor(renderer, 64, 128, 255, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
        }

        SDL_RenderFillRect(renderer, &item);
    }
}

void menu_state_cleanup(void) {
    printf("Finalizando menu principal.\n");
}

void menu_state_enter(void) {
    menu_selection = 0;
}

void menu_state_exit(void) {
}

static int level_selection = 0;

void level_select_state_init(void) {
    printf("Inicializando seleção de fase...\n");
    level_selection = 0;
}

void level_select_state_update(float delta_time) {
    static float auto_timer = 0.0f;
    auto_timer += delta_time;

    if (auto_timer >= 3.0f) {
        state_change_with_transition(STATE_GAMEPLAY, 0.3f);
        auto_timer = 0.0f;
    }
}

void level_select_state_render(void) {
    SDL_Renderer* renderer = engine_get_renderer();

    SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            SDL_Rect level = {
                200 + x * 80,
                200 + y * 80,
                60, 60
            };

            int level_id = y * 3 + x;
            if (level_id == level_selection) {
                SDL_SetRenderDrawColor(renderer, 255, 128, 64, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
            }

            SDL_RenderFillRect(renderer, &level);
        }
    }
}

void level_select_state_cleanup(void) {
    printf("Finalizando seleção de fase.\n");
}

void level_select_state_enter(void) {
    level_selection = 0;
}

void level_select_state_exit(void) {
}

static float ball_x = SCREEN_WIDTH / 2;
static float ball_y = SCREEN_HEIGHT / 2;
static float ball_vel_x = 200.0f;
static float ball_vel_y = 150.0f;
static float paddle_y = SCREEN_HEIGHT / 2;

void gameplay_state_init(void) {
    printf("Inicializando gameplay...\n");

    ball_x = SCREEN_WIDTH / 2;
    ball_y = SCREEN_HEIGHT / 2;
    ball_vel_x = 200.0f;
    ball_vel_y = 150.0f;
    paddle_y = SCREEN_HEIGHT / 2;
}

void gameplay_state_update(float delta_time) {
    ball_x += ball_vel_x * delta_time;
    ball_y += ball_vel_y * delta_time;

    if (ball_y <= 10 || ball_y >= SCREEN_HEIGHT - 10) {
        ball_vel_y = -ball_vel_y;
    }

    if (ball_x <= 0) {
        ball_x = SCREEN_WIDTH / 2;
        ball_y = SCREEN_HEIGHT / 2;
        ball_vel_x = 200.0f;
        ball_vel_y = 150.0f;
    }

    if (ball_x >= SCREEN_WIDTH) {
        ball_vel_x = -ball_vel_x;
    }

    if (ball_y > paddle_y + 30) {
        paddle_y += PADDLE_SPEED * delta_time;
    } else if (ball_y < paddle_y - 30) {
        paddle_y -= PADDLE_SPEED * delta_time;
    }

    if (paddle_y < 50) paddle_y = 50;
    if (paddle_y > SCREEN_HEIGHT - 50) paddle_y = SCREEN_HEIGHT - 50;
}

void gameplay_state_render(void) {
    SDL_Renderer* renderer = engine_get_renderer();

    SDL_SetRenderDrawColor(renderer, 10, 50, 10, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int y = 0; y < SCREEN_HEIGHT; y += 20) {
        SDL_Rect segment = {SCREEN_WIDTH / 2 - 2, y, 4, 10};
        SDL_RenderFillRect(renderer, &segment);
    }

    SDL_Rect ball = {(int)ball_x - 5, (int)ball_y - 5, 10, 10};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball);

    SDL_Rect paddle_player = {SCREEN_WIDTH - 20, (int)paddle_y - 25, 10, 50};
    SDL_RenderFillRect(renderer, &paddle_player);

    SDL_Rect paddle_ai = {10, (int)paddle_y - 25, 10, 50};
    SDL_RenderFillRect(renderer, &paddle_ai);
}

void gameplay_state_cleanup(void) {
    printf("Finalizando gameplay.\n");
}

void gameplay_state_enter(void) {
    ball_x = SCREEN_WIDTH / 2;
    ball_y = SCREEN_HEIGHT / 2;
    paddle_y = SCREEN_HEIGHT / 2;
}

void gameplay_state_exit(void) {
}

void pause_state_init(void) {
    printf("Inicializando pause...\n");
}

void pause_state_update(float delta_time) {
}

void pause_state_render(void) {
    gameplay_state_render();

    SDL_Renderer* renderer = engine_get_renderer();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);

    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);

    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    SDL_Rect pause_menu = {
        SCREEN_WIDTH / 2 - 100,
        SCREEN_HEIGHT / 2 - 50,
        200, 100
    };
    SDL_RenderFillRect(renderer, &pause_menu);
}

void pause_state_cleanup(void) {
    printf("Finalizando pause.\n");
}

void pause_state_enter(void) {
}

void pause_state_exit(void) {
}

void game_over_state_init(void) {
    printf("Inicializando game over...\n");
}

void game_over_state_update(float delta_time) {
    static float auto_timer = 0.0f;
    auto_timer += delta_time;

    if (auto_timer >= 3.0f) {
        state_change_with_transition(STATE_MAIN_MENU, 0.5f);
        auto_timer = 0.0f;
    }
}

void game_over_state_render(void) {
    SDL_Renderer* renderer = engine_get_renderer();

    SDL_SetRenderDrawColor(renderer, 50, 10, 10, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 64, 64, 255);
    SDL_Rect text = {
        SCREEN_WIDTH / 2 - 150,
        SCREEN_HEIGHT / 2 - 25,
        300, 50
    };
    SDL_RenderFillRect(renderer, &text);
}

void game_over_state_cleanup(void) {
    printf("Finalizando game over.\n");
}

void game_over_state_enter(void) {
}

void game_over_state_exit(void) {
}

void victory_state_init(void) {
    printf("Inicializando victory...\n");
}

void victory_state_update(float delta_time) {
    static float auto_timer = 0.0f;
    auto_timer += delta_time;

    if (auto_timer >= 3.0f) {
        state_change_with_transition(STATE_LEVEL_SELECT, 0.5f);
        auto_timer = 0.0f;
    }
}

void victory_state_render(void) {
    SDL_Renderer* renderer = engine_get_renderer();

    SDL_SetRenderDrawColor(renderer, 50, 40, 10, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 215, 64, 255);
    SDL_Rect text = {
        SCREEN_WIDTH / 2 - 150,
        SCREEN_HEIGHT / 2 - 25,
        300, 50
    };
    SDL_RenderFillRect(renderer, &text);
}

void victory_state_cleanup(void) {
    printf("Finalizando victory.\n");
}

void victory_state_enter(void) {
}

void victory_state_exit(void) {
}
