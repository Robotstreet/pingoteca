#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
    Uint8 r, g, b, a;
} TextColor;

typedef enum {
    TEXT_STYLE_SMALL = 0,
    TEXT_STYLE_NORMAL,
    TEXT_STYLE_BOLD,
    TEXT_STYLE_TITLE
} TextStyle;

typedef enum {
    TEXT_ALIGN_LEFT = 0,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_RIGHT
} TextAlign;

typedef struct {
    SDL_Renderer* renderer;
    int font_size;
    bool initialized;
} TextRenderer;

#define TEXT_COLOR_WHITE  ((TextColor){255, 255, 255, 255})
#define TEXT_COLOR_YELLOW ((TextColor){255, 255,   0, 255})
#define TEXT_COLOR_GREEN  ((TextColor){0,   255,   0, 255})
#define TEXT_COLOR_RED    ((TextColor){255,   0,   0, 255})
#define TEXT_COLOR_GRAY   ((TextColor){128, 128, 128, 255})
#define TEXT_COLOR_BLACK  ((TextColor){0,   0,   0, 255})
#define TEXT_COLOR_CYAN   ((TextColor){0, 255, 255, 255})
#define TEXT_COLOR_BLUE   ((TextColor){0,   0, 255, 255})
#define TEXT_COLOR_PURPLE ((TextColor){128,   0, 128, 255})
#define TEXT_COLOR_ORANGE ((TextColor){255, 165,   0, 255})

bool text_renderer_init(SDL_Renderer* renderer);
void text_renderer_cleanup(void);
void text_render_string(const char* text, int x, int y, TextStyle style, TextColor color, TextAlign align);
void text_render_centered(const char* text, int x, int y, int width, TextStyle style, TextColor color);
void text_get_size(const char* text, TextStyle style, int* width, int* height);

#endif /* TEXT_RENDERER_H */
