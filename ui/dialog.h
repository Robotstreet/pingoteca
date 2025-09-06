#ifndef DIALOG_H
#define DIALOG_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#define MAX_DIALOG_TEXTS 5
#define MAX_DIALOG_LENGTH 512
#define MAX_WRAPPED_LINES 6
#define MAX_LINE_LENGTH 128

typedef struct {
    char texts[MAX_DIALOG_TEXTS][MAX_DIALOG_LENGTH];  // Múltiplos textos
    char wrapped_lines[MAX_WRAPPED_LINES][MAX_LINE_LENGTH]; // Linhas com quebra
    int total_dialogs;          // Quantos diálogos há no total
    int current_dialog;         // Diálogo atual sendo exibido
    int wrapped_line_count;     // Quantas linhas o texto atual tem
    bool active;
    bool finished;
    bool waiting_for_input;     // Esperando input para próximo diálogo
    float timer;
    float auto_advance_time;    // Tempo para avançar automaticamente (0 = manual)
    
    bool show_portraits;        // Se deve mostrar portraits
    char current_speaker[32];   // Nome do speaker atual ("player", "inimigo1", etc.)
    SDL_Texture* current_portrait; // Texture do portrait atual
    bool portrait_on_right;     // true = direita (inimigo), false = esquerda (jogador)
} LevelDialog;

void dialog_init(LevelDialog* dialog);
void dialog_start_single(LevelDialog* dialog, const char* text, float auto_advance_time);
void dialog_start_multiple(LevelDialog* dialog, const char* texts[], int count, float auto_advance_time);
void dialog_update(LevelDialog* dialog, float delta_time);
void dialog_render(LevelDialog* dialog, SDL_Renderer* renderer);
void dialog_advance(LevelDialog* dialog);  // Avança para próximo diálogo
bool dialog_is_finished(LevelDialog* dialog);
bool dialog_is_waiting_for_input(LevelDialog* dialog);

void dialog_set_speaker(LevelDialog* dialog, const char* speaker, bool is_enemy);

void dialog_wrap_text(LevelDialog* dialog, const char* text);
void dialog_setup_current_text(LevelDialog* dialog);

#endif
