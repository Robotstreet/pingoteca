#include "engine.h"
#include <stdio.h>
#include <stdlib.h>

Engine g_engine = {0};

static EngineEventCallback g_event_callback = NULL;

bool engine_init(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        return false;
    }

    g_engine.window = SDL_CreateWindow(
        SCREEN_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!g_engine.window) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    g_engine.renderer = SDL_CreateRenderer(
        g_engine.window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!g_engine.renderer) {
        printf("Erro ao criar renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(g_engine.window);
        SDL_Quit();
        return false;
    }

    g_engine.running = true;
    g_engine.paused = false;
    g_engine.delta_time = 0.0f;
    g_engine.fixed_delta_time = 1.0f / SCREEN_FPS;
    g_engine.last_frame_time = SDL_GetTicks();
    g_engine.frame_count = 0;
    g_engine.fps = 0.0f;
    g_engine.fps_timer = SDL_GetTicks();
    g_engine.fps_counter = 0;
    g_engine.debug_mode = false;
    g_engine.show_fps = false;

    printf("Motor inicializado com sucesso!\n");
    return true;
}

void engine_run(void) {
    states_init();

    while (g_engine.running) {
        g_engine.frame_start_time = SDL_GetTicks();

        engine_update_timing();

        engine_handle_events();

        if (!g_engine.paused) {
            states_update(g_engine.delta_time);
        }

        engine_clear_screen();
        states_render();

        if (g_engine.show_fps) {
            engine_render_debug_info();
        }

        engine_present_screen();

        Uint32 frame_time = SDL_GetTicks() - g_engine.frame_start_time;
        if (frame_time < g_engine.fixed_delta_time * 1000) {
            SDL_Delay((Uint32)(g_engine.fixed_delta_time * 1000) - frame_time);
        }
    }

    states_cleanup();
    engine_shutdown();
}

void engine_shutdown(void) {
    if (g_engine.renderer) {
        SDL_DestroyRenderer(g_engine.renderer);
        g_engine.renderer = NULL;
    }

    if (g_engine.window) {
        SDL_DestroyWindow(g_engine.window);
        g_engine.window = NULL;
    }

    SDL_Quit();
    printf("Motor finalizado.\n");
}

void engine_pause(void) {
    g_engine.paused = true;
}

void engine_resume(void) {
    g_engine.paused = false;
}

void engine_quit(void) {
    g_engine.running = false;
}

void engine_update_timing(void) {
    Uint32 current_time = SDL_GetTicks();
    g_engine.delta_time = (current_time - g_engine.last_frame_time) / 1000.0f;
    g_engine.last_frame_time = current_time;

    g_engine.frame_count++;
    g_engine.fps_counter++;

    if (current_time - g_engine.fps_timer >= 1000) {
        g_engine.fps = g_engine.fps_counter;
        g_engine.fps_counter = 0;
        g_engine.fps_timer = current_time;
    }
}

float engine_get_delta_time(void) {
    return g_engine.delta_time;
}

float engine_get_fps(void) {
    return g_engine.fps;
}

Uint32 engine_get_ticks(void) {
    return SDL_GetTicks();
}

void engine_clear_screen(void) {
    SDL_SetRenderDrawColor(g_engine.renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_engine.renderer);
}

void engine_present_screen(void) {
    SDL_RenderPresent(g_engine.renderer);
}

SDL_Renderer* engine_get_renderer(void) {
    return g_engine.renderer;
}

void engine_handle_events(void) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                engine_quit();
                break;

            case SDL_WINDOWEVENT:
                engine_handle_window_event(&event.window);
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_F1:
                        engine_toggle_debug();
                        break;
                    case SDLK_F2:
                        engine_toggle_fps();
                        break;
                    case SDLK_ESCAPE:
                        if (state_get_current() == STATE_GAMEPLAY) {
                            state_change(STATE_PAUSE);
                        } else if (state_get_current() == STATE_PAUSE) {
                            state_change(STATE_GAMEPLAY);
                        }
                        break;
                }
                break;
        }

        if (g_event_callback) {
            g_event_callback(&event);
        }
    }
}

void engine_handle_window_event(SDL_WindowEvent* event) {
    switch (event->event) {
        case SDL_WINDOWEVENT_CLOSE:
            engine_quit();
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            engine_pause();
            break;
        case SDL_WINDOWEVENT_RESTORED:
            engine_resume();
            break;
    }
}

void engine_toggle_debug(void) {
    g_engine.debug_mode = !g_engine.debug_mode;
    printf("Modo debug: %s\n", g_engine.debug_mode ? "ON" : "OFF");
}

void engine_toggle_fps(void) {
    g_engine.show_fps = !g_engine.show_fps;
}

void engine_render_debug_info(void) {
    static float debug_timer = 0.0f;
    debug_timer += g_engine.delta_time;

    if (debug_timer >= 1.0f) {
        printf("FPS: %.1f | Delta: %.4f | Frame: %d\n",
               g_engine.fps, g_engine.delta_time, g_engine.frame_count);
        debug_timer = 0.0f;
    }
}

void engine_set_event_callback(EngineEventCallback callback) {
    g_event_callback = callback;
}
