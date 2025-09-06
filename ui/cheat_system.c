#include "cheat_system.h"
#include "text_renderer.h"
#include <string.h>
#include <stdio.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

static void cheat_activate_code(CheatSystem* cheat, const char* code);
static void cheat_show_confirmation(CheatSystem* cheat, const char* cheat_name);
static void cheat_apply_unlock_all(CheatSystem* cheat);
static void cheat_apply_freeze_ai(CheatSystem* cheat);
static void cheat_apply_godmode(CheatSystem* cheat);
static void cheat_apply_super_ball(CheatSystem* cheat);
static void cheat_apply_noclip(CheatSystem* cheat);
static void cheat_apply_infinite_power(CheatSystem* cheat);
static void cheat_apply_instant_win(CheatSystem* cheat);
static void cheat_apply_rainbow_mode(CheatSystem* cheat);
static void cheat_apply_slow_motion(CheatSystem* cheat);
static void cheat_apply_giant_paddle(CheatSystem* cheat);
static void cheat_apply_ball_control(CheatSystem* cheat);


void cheat_system_init(CheatSystem* cheat) {
    memset(cheat, 0, sizeof(CheatSystem));
    cheat->active = true;
    cheat->interface_open = false;
    cheat->input_length = 0;
    cheat->active_boxes_count = 0;
    cheat->current_context = CHEAT_CONTEXT_GAMEPLAY;
    
    for (int i = 0; i < MAX_CONFIRMATION_BOXES; i++) {
        cheat->confirmation_boxes[i].active = false;
        cheat->confirmation_boxes[i].timer = 0.0f;
        cheat->confirmation_boxes[i].animation_offset = 100.0f;
        cheat->confirmation_boxes[i].target_y = 0.0f;
    }
    
    cheat->all_levels_unlocked = false;
    cheat->ai_frozen = false;
    cheat->godmode_paddle = false;
    cheat->super_ball = false;
    cheat->noclip_mode = false;
    cheat->infinite_power = false;
    cheat->rainbow_mode = false;
    cheat->slow_motion = false;
    cheat->giant_paddle = false;
    cheat->instant_win = false;
    cheat->ball_control = false; 
}

void cheat_system_update(CheatSystem* cheat, float delta_time) {
    if (!cheat->active) return;
    
    for (int i = 0; i < MAX_CONFIRMATION_BOXES; i++) {
        ConfirmationBox* box = &cheat->confirmation_boxes[i];
        if (box->active) {
            box->timer -= delta_time;
            
            if (box->animation_offset > 0) {
                box->animation_offset -= delta_time * 200.0f; // Velocidade da animação
                if (box->animation_offset < 0) {
                    box->animation_offset = 0;
                }
            }
            
            if (box->timer <= 0.0f) {
                box->active = false;
                
                for (int j = i; j < MAX_CONFIRMATION_BOXES - 1; j++) {
                    if (cheat->confirmation_boxes[j + 1].active) {
                        cheat->confirmation_boxes[j] = cheat->confirmation_boxes[j + 1];
                        cheat->confirmation_boxes[j + 1].active = false;
                    }
                }
                
                cheat->active_boxes_count = 0;
                for (int k = 0; k < MAX_CONFIRMATION_BOXES; k++) {
                    if (cheat->confirmation_boxes[k].active) {
                        cheat->active_boxes_count++;
                    }
                }
                
                for (int k = 0; k < cheat->active_boxes_count; k++) {
                    cheat->confirmation_boxes[k].target_y = SCREEN_HEIGHT - 80 - (k * 70);
                }
            }
        }
    }
}

void cheat_system_render(CheatSystem* cheat, SDL_Renderer* renderer) {
    if (!cheat->active) return;
    
    if (cheat->interface_open) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        
        if (cheat->current_context == CHEAT_CONTEXT_GAMEPLAY) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
            SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderFillRect(renderer, &overlay);
            
            SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
        } else {
            switch (cheat->current_context) {
                case CHEAT_CONTEXT_MENU:
                    for (int y = 0; y < SCREEN_HEIGHT; y++) {
                        int color = 20 + (y * 30) / SCREEN_HEIGHT;
                        SDL_SetRenderDrawColor(renderer, color, color, color + 20, 180);
                        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
                    }
                    SDL_SetRenderDrawColor(renderer, 64, 64, 84, 255);
                    break;
                    
                case CHEAT_CONTEXT_LEVEL_SELECT:
                    for (int y = 0; y < SCREEN_HEIGHT; y++) {
                        int color = 30 + (y * 20) / SCREEN_HEIGHT;
                        SDL_SetRenderDrawColor(renderer, color, color, color + 20, 180);
                        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
                    }
                    SDL_SetRenderDrawColor(renderer, 74, 74, 94, 255);
                    break;
                    
                case CHEAT_CONTEXT_OPTIONS:
                    for (int y = 0; y < SCREEN_HEIGHT; y++) {
                        int color = 25 + (y * 25) / SCREEN_HEIGHT;
                        SDL_SetRenderDrawColor(renderer, color, color, color + 15, 180);
                        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
                    }
                    SDL_SetRenderDrawColor(renderer, 69, 69, 89, 255);
                    break;
                    
                default:
                    SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
                    break;
            }
        }
        
        SDL_Rect cheat_window = {
            SCREEN_WIDTH / 2 - 200,
            SCREEN_HEIGHT / 2 - 50,
            400, 100
        };
        SDL_RenderFillRect(renderer, &cheat_window);
        
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        SDL_RenderDrawRect(renderer, &cheat_window);
        
        TextColor title_color = TEXT_COLOR_WHITE;
        text_render_centered("CODIGO DE TRAPACA:", 
                           cheat_window.x, cheat_window.y + 10, 
                           cheat_window.w, TEXT_STYLE_NORMAL, title_color);
        
        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_Rect input_field = {
            cheat_window.x + 20,
            cheat_window.y + 40,
            cheat_window.w - 40, 30
        };
        SDL_RenderFillRect(renderer, &input_field);
        
        SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
        SDL_RenderDrawRect(renderer, &input_field);
        
        char display_text[MAX_CHEAT_INPUT + 2];
        snprintf(display_text, sizeof(display_text), "%s_", cheat->input_buffer);
        TextColor input_color = TEXT_COLOR_YELLOW;
        text_render_string(display_text, input_field.x + 5, input_field.y + 8, 
                          TEXT_STYLE_NORMAL, input_color, TEXT_ALIGN_LEFT);
        
        TextColor instruction_color = TEXT_COLOR_GRAY;
        text_render_centered("ENTER PARA ATIVAR - ESC PARA FECHAR", 
                           cheat_window.x, cheat_window.y + 75, 
                           cheat_window.w, TEXT_STYLE_SMALL, instruction_color);
    }
    
    for (int i = 0; i < MAX_CONFIRMATION_BOXES; i++) {
        ConfirmationBox* box = &cheat->confirmation_boxes[i];
        if (box->active) {
            int box_width = 280;
            int box_height = 60;
            int margin = 20;
            
            float current_y = SCREEN_HEIGHT - 80 - (i * 70) + box->animation_offset;
            
            SDL_Rect confirm_modal = {
                SCREEN_WIDTH - box_width - margin,
                (int)current_y,
                box_width,
                box_height
            };
            
            SDL_SetRenderDrawColor(renderer, 0, 120, 0, 240);
            SDL_RenderFillRect(renderer, &confirm_modal);
            
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderDrawRect(renderer, &confirm_modal);
            
            TextColor confirm_color = TEXT_COLOR_WHITE;
            text_render_centered("CHEAT ATIVADO!", 
                               confirm_modal.x, confirm_modal.y + 8, 
                               confirm_modal.w, TEXT_STYLE_SMALL, confirm_color);
            
            TextColor cheat_name_color = TEXT_COLOR_GREEN;
            text_render_centered(box->text, 
                               confirm_modal.x, confirm_modal.y + 25, 
                               confirm_modal.w, TEXT_STYLE_SMALL, cheat_name_color);
            
            float time_progress = box->timer / 6.0f;
            int bar_width = (int)(box_width * 0.8f * time_progress);
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 200);
            SDL_Rect time_bar = {
                confirm_modal.x + 10,
                confirm_modal.y + box_height - 8,
                bar_width,
                4
            };
            SDL_RenderFillRect(renderer, &time_bar);
        }
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void cheat_system_handle_input(CheatSystem* cheat, SDL_Event* event) {
    if (!cheat->active || !cheat->interface_open) return;
    
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.sym) {
            case SDLK_ESCAPE:
                cheat->interface_open = false;
                cheat->input_length = 0;
                memset(cheat->input_buffer, 0, sizeof(cheat->input_buffer));
                break;
                
            case SDLK_RETURN:
                if (cheat->input_length > 0) {
                    cheat_activate_code(cheat, cheat->input_buffer);
                    cheat->input_length = 0;
                    memset(cheat->input_buffer, 0, sizeof(cheat->input_buffer));
                    cheat->interface_open = false;
                }
                break;
                
            case SDLK_BACKSPACE:
                if (cheat->input_length > 0) {
                    cheat->input_length--;
                    cheat->input_buffer[cheat->input_length] = '\0';
                }
                break;
                
            default:
                if (cheat->input_length < MAX_CHEAT_INPUT - 1) {
                    char c = event->key.keysym.sym;
                    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
                        (c >= '0' && c <= '9') || c == '_') {
                        if (c >= 'a' && c <= 'z') {
                            c = c - 'a' + 'A';
                        }
                        cheat->input_buffer[cheat->input_length] = c;
                        cheat->input_length++;
                        cheat->input_buffer[cheat->input_length] = '\0';
                    }
                }
                break;
        }
    }
}

void cheat_system_toggle_interface(CheatSystem* cheat) {
    if (!cheat->active) return;
    
    cheat->interface_open = !cheat->interface_open;
    if (cheat->interface_open) {
        cheat->input_length = 0;
        memset(cheat->input_buffer, 0, sizeof(cheat->input_buffer));
    }
}

bool cheat_system_is_interface_open(CheatSystem* cheat) {
    return cheat->active && cheat->interface_open;
}

static void cheat_activate_code(CheatSystem* cheat, const char* code) {
    if (strcmp(code, "UNLOCKALL") == 0) {
        cheat_apply_unlock_all(cheat);
        cheat_show_confirmation(cheat, "TODOS OS NIVEIS DESBLOQUEADOS");
    }
    else if (strcmp(code, "FREEZEAI") == 0) {
        cheat_apply_freeze_ai(cheat);
        cheat_show_confirmation(cheat, "INIMIGOS CONGELADOS");
    }
    else if (strcmp(code, "GODMODE") == 0) {
        cheat_apply_godmode(cheat);
        cheat_show_confirmation(cheat, "MODO DEUS ATIVADO");
    }
    else if (strcmp(code, "SUPERBALL") == 0) {
        cheat_apply_super_ball(cheat);
        cheat_show_confirmation(cheat, "BOLA SUPER RAPIDA");
    }
    else if (strcmp(code, "NOCLIP") == 0) {
        cheat_apply_noclip(cheat);
        cheat_show_confirmation(cheat, "MODO FANTASMA ATIVADO");
    }
    else if (strcmp(code, "INFINITEPOWER") == 0) {
        cheat_apply_infinite_power(cheat);
        cheat_show_confirmation(cheat, "POWER-UPS INFINITOS");
    }
    else if (strcmp(code, "INSTANTWIN") == 0) {
        cheat_apply_instant_win(cheat);
        cheat_show_confirmation(cheat, "VITORIA INSTANTANEA");
    }
    else if (strcmp(code, "RAINBOW") == 0) {
        cheat_apply_rainbow_mode(cheat);
        cheat_show_confirmation(cheat, "MODO ARCO-IRIS");
    }
    else if (strcmp(code, "SLOWMO") == 0) {
        cheat_apply_slow_motion(cheat);
        cheat_show_confirmation(cheat, "CAMERA LENTA");
    }
    else if (strcmp(code, "GIANT") == 0) {
        cheat_apply_giant_paddle(cheat);
        cheat_show_confirmation(cheat, "RAQUETE GIGANTE");
    }
    else if (strcmp(code, "BALLCONTROL") == 0) {
        cheat_apply_ball_control(cheat);
        cheat_show_confirmation(cheat, "CONTROLE DA BOLA");
    }
    else {
        cheat_show_confirmation(cheat, "CODIGO INVALIDO");
    }
}

static void cheat_show_confirmation(CheatSystem* cheat, const char* cheat_name) {
    int slot = -1;
    for (int i = 0; i < MAX_CONFIRMATION_BOXES; i++) {
        if (!cheat->confirmation_boxes[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        slot = MAX_CONFIRMATION_BOXES - 1;
        for (int i = 0; i < MAX_CONFIRMATION_BOXES - 1; i++) {
            cheat->confirmation_boxes[i] = cheat->confirmation_boxes[i + 1];
        }
    }
    
    ConfirmationBox* box = &cheat->confirmation_boxes[slot];
    box->active = true;
    box->timer = 6.0f; // 6 segundos
    box->animation_offset = 100.0f; // Começa de baixo
    strncpy(box->text, cheat_name, sizeof(box->text) - 1);
    box->text[sizeof(box->text) - 1] = '\0';
    
    cheat->active_boxes_count = 0;
    for (int i = 0; i < MAX_CONFIRMATION_BOXES; i++) {
        if (cheat->confirmation_boxes[i].active) {
            cheat->active_boxes_count++;
        }
    }
}

void cheat_system_set_context(CheatSystem* cheat, CheatContext context) {
    if (!cheat) return;
    cheat->current_context = context;
}

static void cheat_apply_unlock_all(CheatSystem* cheat) {
    cheat->all_levels_unlocked = true;
}

static void cheat_apply_freeze_ai(CheatSystem* cheat) {
    cheat->ai_frozen = true;
}

static void cheat_apply_godmode(CheatSystem* cheat) {
    cheat->godmode_paddle = true;
}

static void cheat_apply_super_ball(CheatSystem* cheat) {
    cheat->super_ball = true;
}

static void cheat_apply_ball_control(CheatSystem* cheat) {
    cheat->ball_control = true;
}

static void cheat_apply_noclip(CheatSystem* cheat) {
    cheat->noclip_mode = true;
}

static void cheat_apply_infinite_power(CheatSystem* cheat) {
    cheat->infinite_power = true;
}

static void cheat_apply_instant_win(CheatSystem* cheat) {
    cheat->instant_win = true;
}

static void cheat_apply_rainbow_mode(CheatSystem* cheat) {
    cheat->rainbow_mode = true;
}

static void cheat_apply_slow_motion(CheatSystem* cheat) {
    cheat->slow_motion = true;
}

static void cheat_apply_giant_paddle(CheatSystem* cheat) {
    cheat->giant_paddle = true;
}
