#ifndef ENGINE_H
#define ENGINE_H

#include "config.h"
#include "states.h"
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct Engine {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    bool paused;

    float delta_time;
    float fixed_delta_time;
    Uint32 last_frame_time;
    Uint32 frame_start_time;
    int frame_count;
    float fps;

    Uint32 fps_timer;
    int fps_counter;

    bool debug_mode;
    bool show_fps;
} Engine;

extern Engine g_engine;

bool engine_init(void);
void engine_run(void);
void engine_shutdown(void);

void engine_pause(void);
void engine_resume(void);
void engine_quit(void);

void engine_update_timing(void);
float engine_get_delta_time(void);
float engine_get_fps(void);
Uint32 engine_get_ticks(void);

void engine_clear_screen(void);
void engine_present_screen(void);
SDL_Renderer* engine_get_renderer(void);

void engine_handle_events(void);
void engine_handle_window_event(SDL_WindowEvent* event);

void engine_toggle_debug(void);
void engine_toggle_fps(void);
void engine_render_debug_info(void);

typedef void (*EngineEventCallback)(SDL_Event* event);
void engine_set_event_callback(EngineEventCallback callback);

#endif // ENGINE_H
