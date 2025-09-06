#include "dialog.h"
#include "text_renderer.h"
#include <string.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>


#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 768
#define DIALOG_HEIGHT 150
#define DIALOG_MARGIN 10
#define TEXT_MARGIN 20
#define CHARS_PER_LINE 50 

void dialog_init(LevelDialog* dialog) {
    memset(dialog, 0, sizeof(LevelDialog));
    dialog->active = false;
    dialog->finished = true;
    dialog->waiting_for_input = false;
    dialog->total_dialogs = 0;
    dialog->current_dialog = 0;
    dialog->wrapped_line_count = 0;
    dialog->timer = 0.0f;
    dialog->auto_advance_time = 0.0f;
}

void dialog_start_single(LevelDialog* dialog, const char* text, float auto_advance_time) {
    const char* texts[] = {text};
    dialog_start_multiple(dialog, texts, 1, auto_advance_time);
}

void dialog_start_multiple(LevelDialog* dialog, const char* texts[], int count, float auto_advance_time) {
    if (!dialog || !texts || count <= 0 || count > MAX_DIALOG_TEXTS) {
        printf("Erro: Parâmetros inválidos para dialog_start_multiple\n");
        return;
    }

    dialog_init(dialog);

    dialog->total_dialogs = count;
    for (int i = 0; i < count; i++) {
        strncpy(dialog->texts[i], texts[i], MAX_DIALOG_LENGTH - 1);
        dialog->texts[i][MAX_DIALOG_LENGTH - 1] = '\0';
    }

    dialog->active = true;
    dialog->finished = false;
    dialog->current_dialog = 0;
    dialog->auto_advance_time = auto_advance_time;
    dialog->timer = 0.0f;
    dialog->waiting_for_input = (auto_advance_time <= 0.0f);

    dialog_setup_current_text(dialog);

    printf("Diálogo iniciado: %d texto(s), auto-advance: %.1fs\n", count, auto_advance_time);
}

void dialog_update(LevelDialog* dialog, float delta_time) {
    if (!dialog->active) return;

    dialog->timer += delta_time;

    if (dialog->auto_advance_time > 0.0f && !dialog->waiting_for_input) {
        if (dialog->timer >= dialog->auto_advance_time) {
            dialog_advance(dialog);
        }
    }
}
static SDL_Texture* dialog_load_portrait(SDL_Renderer* renderer, const char* character_name) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "assets/image/characters/%s.png", character_name);
    
    SDL_Surface* surface = IMG_Load(filepath);
    if (!surface) {
        printf("Aviso: Não foi possível carregar portrait %s\n", filepath);
        return NULL;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!texture) {
        printf("Erro: Não foi possível criar texture do portrait %s\n", filepath);
    }
    
    return texture;
}

void dialog_set_speaker(LevelDialog* dialog, const char* speaker, bool is_enemy) {
    if (!dialog || !speaker) return;
    
    strncpy(dialog->current_speaker, speaker, sizeof(dialog->current_speaker) - 1);
    dialog->current_speaker[sizeof(dialog->current_speaker) - 1] = '\0';
    dialog->portrait_on_right = is_enemy;
    dialog->show_portraits = true;
    
    printf("Speaker configurado: %s (lado %s)\n", speaker, is_enemy ? "direito" : "esquerdo");
}

void dialog_render(LevelDialog* dialog, SDL_Renderer* renderer) {
    if (!dialog->active) return;

    SDL_Rect dialog_box = {
        0,                                    
        SCREEN_HEIGHT - DIALOG_HEIGHT,       
        SCREEN_WIDTH,                         
        DIALOG_HEIGHT                         
    };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
    SDL_Rect screen_overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT - DIALOG_HEIGHT};
    SDL_RenderFillRect(renderer, &screen_overlay);

    if (dialog->show_portraits && strlen(dialog->current_speaker) > 0) {
        char portrait_filename[256];
        bool is_player = (strcmp(dialog->current_speaker, "jogador") == 0);
        
        if (is_player) {
            snprintf(portrait_filename, sizeof(portrait_filename), "assets/image/characters/jogador.png");
        } else {
            int enemy_number = 1; // Padrão
            if (strcmp(dialog->current_speaker, "inimigo1") == 0) enemy_number = 1;
            else if (strcmp(dialog->current_speaker, "inimigo5") == 0) enemy_number = 2;
            else if (strcmp(dialog->current_speaker, "inimigo9") == 0) enemy_number = 3;
            
            snprintf(portrait_filename, sizeof(portrait_filename), "assets/image/characters/inimigo%d.png", enemy_number);
        }
        
        SDL_Surface* portrait_surface = IMG_Load(portrait_filename);
        if (portrait_surface) {
            SDL_Texture* portrait_texture = SDL_CreateTextureFromSurface(renderer, portrait_surface);
            SDL_FreeSurface(portrait_surface);
            
            if (portrait_texture) {
                int portrait_width = 180;
                int portrait_height = 250;
                
                SDL_Rect portrait_rect;
                if (is_player) {
                    portrait_rect.x = 10;
                } else {
                    portrait_rect.x = SCREEN_WIDTH - portrait_width - 10;
                }
                
                portrait_rect.y = dialog_box.y - (portrait_height - 50); // 50px ficam atrás da caixa
                portrait_rect.w = portrait_width;
                portrait_rect.h = portrait_height;
                
                SDL_RenderCopy(renderer, portrait_texture, NULL, &portrait_rect);
                
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                for (int i = 0; i < 3; i++) {
                    SDL_Rect border_rect = {
                        portrait_rect.x - i, portrait_rect.y - i,
                        portrait_rect.w + 2*i, portrait_rect.h + 2*i
                    };
                    SDL_RenderDrawRect(renderer, &border_rect);
                }
                
                SDL_DestroyTexture(portrait_texture);
            }
        }
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    
    for (int y = 0; y < dialog_box.h; y++) {
        int intensity = 40 + (y * 60) / dialog_box.h;
        SDL_SetRenderDrawColor(renderer, intensity * 0.4f, intensity * 0.4f, intensity * 0.6f, 255);
        SDL_RenderDrawLine(renderer, dialog_box.x, dialog_box.y + y, 
                          dialog_box.x + dialog_box.w, dialog_box.y + y);
    }

    SDL_SetRenderDrawColor(renderer, 150, 150, 200, 255);
    for (int i = 0; i < 3; i++) {
        SDL_RenderDrawLine(renderer, dialog_box.x, dialog_box.y + i, 
                          dialog_box.x + dialog_box.w, dialog_box.y + i);
    }

    int line_height = 20;
    int start_y = dialog_box.y + TEXT_MARGIN;
    
    for (int i = 0; i < dialog->wrapped_line_count; i++) {
        int y_pos = start_y + (i * line_height);
        
        if (y_pos + line_height > dialog_box.y + dialog_box.h - TEXT_MARGIN) {
            break;
        }

        TextColor text_color = TEXT_COLOR_WHITE;
        text_render_string(dialog->wrapped_lines[i], 
                          dialog_box.x + TEXT_MARGIN, 
                          y_pos, 
                          TEXT_STYLE_NORMAL, 
                          text_color, 
                          TEXT_ALIGN_LEFT);
    }

    int indicator_y = dialog_box.y + dialog_box.h - 35;
    
    if (dialog->total_dialogs > 1) {
        char progress_text[64];
        snprintf(progress_text, sizeof(progress_text), "%d / %d", 
                dialog->current_dialog + 1, dialog->total_dialogs);
        
        TextColor progress_color = TEXT_COLOR_YELLOW;
        text_render_string(progress_text, 
                          dialog_box.x + TEXT_MARGIN, 
                          indicator_y, 
                          TEXT_STYLE_SMALL, 
                          progress_color, 
                          TEXT_ALIGN_LEFT);
    }

    const char* instruction = "";
    if (dialog->waiting_for_input || dialog->auto_advance_time <= 0.0f) {
        if (dialog->current_dialog < dialog->total_dialogs - 1) {
            instruction = "PRESSIONE QUALQUER BOTAO PARA CONTINUAR";
        } else {
            instruction = "PRESSIONE QUALQUER BOTAO PARA FECHAR";
        }
    } else {
        float remaining = dialog->auto_advance_time - dialog->timer;
        if (remaining > 0) {
            static char time_text[64];
            snprintf(time_text, sizeof(time_text), "FECHANDO EM %.1fs", remaining);
            instruction = time_text;
        }
    }

    if (strlen(instruction) > 0) {
        TextColor instruction_color = TEXT_COLOR_GRAY;
        text_render_centered(instruction, 
                           SCREEN_WIDTH / 2, 
                           indicator_y, 
                           SCREEN_WIDTH / 2 - TEXT_MARGIN, 
                           TEXT_STYLE_SMALL, 
                           instruction_color);
    }

    static float blink_timer = 0;
    blink_timer += 0.02f;
    
    if (dialog->waiting_for_input && sinf(blink_timer * 3.0f) > 0) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);
        SDL_Rect blink_indicator = {
            SCREEN_WIDTH - 20, 
            dialog_box.y + 10, 
            10, 10
        };
        SDL_RenderFillRect(renderer, &blink_indicator);
    }
}

void dialog_advance(LevelDialog* dialog) {
    if (!dialog->active) return;

    dialog->current_dialog++;
    dialog->timer = 0.0f;

    if (dialog->current_dialog >= dialog->total_dialogs) {
        dialog->active = false;
        dialog->finished = true;
        dialog->waiting_for_input = false;
        dialog->show_portraits = false;
        strcpy(dialog->current_speaker, ""); // Limpar speaker
        printf("Todos os diálogos finalizados\n");
    } else {
        dialog->waiting_for_input = (dialog->auto_advance_time <= 0.0f);
        dialog_setup_current_text(dialog);
        
        if (dialog->show_portraits) {
            if (dialog->current_dialog % 2 == 0) {
                dialog_set_speaker(dialog, "jogador", false);
            } else {
                if (strstr(dialog->texts[dialog->current_dialog], "novato") || 
                    strstr(dialog->texts[dialog->current_dialog], "habilidade")) {
                    dialog_set_speaker(dialog, "inimigo1", true);
                } else if (strstr(dialog->texts[dialog->current_dialog], "poderes") ||
                          strstr(dialog->texts[dialog->current_dialog], "magia")) {
                    dialog_set_speaker(dialog, "inimigo5", true);
                } else if (strstr(dialog->texts[dialog->current_dialog], "MESTRE") ||
                          strstr(dialog->texts[dialog->current_dialog], "definitivo")) {
                    dialog_set_speaker(dialog, "inimigo9", true);
                } else {
                    dialog_set_speaker(dialog, "inimigo1", true); // Fallback
                }
            }
        }
        
        printf("Avançando para diálogo %d/%d - Speaker: %s\n", 
               dialog->current_dialog + 1, dialog->total_dialogs, dialog->current_speaker);
    }
}

bool dialog_is_finished(LevelDialog* dialog) {
    return dialog->finished;
}

bool dialog_is_waiting_for_input(LevelDialog* dialog) {
    return dialog->active && dialog->waiting_for_input;
}

void dialog_wrap_text(LevelDialog* dialog, const char* text) {
    if (!text || strlen(text) == 0) {
        dialog->wrapped_line_count = 0;
        return;
    }

    dialog->wrapped_line_count = 0;
    int text_len = strlen(text);
    int current_pos = 0;

    while (current_pos < text_len && dialog->wrapped_line_count < MAX_WRAPPED_LINES) {
        int line_start = current_pos;
        int line_length = 0;
        int last_space = -1;

        while (current_pos < text_len && line_length < CHARS_PER_LINE) {
            char c = text[current_pos];
            
            if (c == ' ') {
                last_space = current_pos;
            } else if (c == '\n') {
                break;
            }
            
            current_pos++;
            line_length++;
        }

        int line_end = current_pos;
        if (current_pos < text_len && text[current_pos] != '\n') {
            if (last_space > line_start) {
                line_end = last_space;
                current_pos = last_space + 1; // Pular o espaço
            }
        } else if (current_pos < text_len && text[current_pos] == '\n') {
            current_pos++;
        }

        int copy_length = line_end - line_start;
        if (copy_length > 0 && copy_length < MAX_LINE_LENGTH - 1) {
            strncpy(dialog->wrapped_lines[dialog->wrapped_line_count], 
                   text + line_start, copy_length);
            dialog->wrapped_lines[dialog->wrapped_line_count][copy_length] = '\0';
            dialog->wrapped_line_count++;
        }

        while (current_pos < text_len && text[current_pos] == ' ') {
            current_pos++;
        }
    }

    printf("Texto quebrado em %d linhas\n", dialog->wrapped_line_count);
}

void dialog_setup_current_text(LevelDialog* dialog) {
    if (dialog->current_dialog < dialog->total_dialogs) {
        dialog_wrap_text(dialog, dialog->texts[dialog->current_dialog]);
    }
}
