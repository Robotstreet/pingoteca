#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define SCREEN_FPS 60
#define SCREEN_TITLE "Pingoteca - O Ping Pong Viciante"

#define BALL_SPEED_MIN 200.0f
#define BALL_SPEED_MAX 800.0f
#define BALL_SPEED_INCREMENT 50.0f
#define PADDLE_SPEED 400.0f
#define FRICTION 0.99f

#define MAX_LIVES 3
#define MAX_LEVELS 9
#define WORLDS 3
#define LEVELS_PER_WORLD 3

#define MAX_SOUNDS 32
#define MAX_MUSIC_TRACKS 8
#define MASTER_VOLUME 0.8f
#define MUSIC_VOLUME 0.6f
#define SFX_VOLUME 0.8f

#define MAX_PARTICLES 200
#define PARTICLE_LIFETIME 2.0f

#define ASSETS_PATH "assets/"
#define TEXTURES_PATH "assets/textures/"
#define SOUNDS_PATH "assets/sounds/"
#define MUSIC_PATH "assets/music/"
#define FONTS_PATH "assets/fonts/"
#define DATA_PATH "data/"

#define COLOR_WHITE {255, 255, 255, 255}
#define COLOR_BLACK {0, 0, 0, 255}
#define COLOR_BLUE {64, 128, 255, 255}
#define COLOR_RED {255, 64, 64, 255}
#define COLOR_GREEN {64, 255, 64, 255}
#define COLOR_YELLOW {255, 255, 64, 255}
#define COLOR_PURPLE {128, 64, 255, 255}
#define COLOR_ORANGE {255, 128, 64, 255}

typedef enum {
    STATE_SPLASH,
    STATE_MAIN_MENU,
    STATE_LEVEL_SELECT,
    STATE_GAMEPLAY,
    STATE_PAUSE,
    STATE_VICTORY,
    STATE_GAME_OVER,
    STATE_PVP_CONFIG,     // NOVO
    STATE_PVP_GAMEPLAY,   // NOVO
    STATE_PVP_VICTORY,    // NOVO
    STATE_PVP_DEFEAT,     // NOVO
    STATE_EXIT
} GameState;

typedef struct {
    bool fullscreen;
    bool vsync;
    float master_volume;
    float music_volume;
    float sfx_volume;
    int difficulty;
    bool show_fps;
    bool debug_mode;
} Config;

extern Config g_config;

void config_init(void);
void config_load(void);
void config_save(void);
void config_reset_to_default(void);

#endif // CONFIG_H
