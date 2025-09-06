#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


#include "utils/string_utils.h"
#include "utils/file_utils.h"
#include "render/visual_effects.h"
#include "physics/ball.h"
#include "utils/string_utils.h"
#include "utils/file_utils.h"
#include "render/visual_effects.h"
#include "core/engine.h"
#include "core/config.h"
#include "ui/text_renderer.h"
#include "game/enemy.h"
#include "content/levels_data.h"
#include "audio/audio.h"
#include "ui/dialog.h"
#include "ui/cheat_system.h"



#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 100
#define BALL_SIZE 15
#define MAX_LEVELS 9
#define FPS 60


typedef struct {
    float x, y;
    int type; // 0=speed boost, 1=size increase, 2=multi-ball
    bool active;
    float timer;
} PowerUp;

typedef struct {
    int id;
    char name[64];
    char theme[32];
    float ball_speed_multiplier;
    int ai_difficulty;
    bool unlocked;
    bool completed;  
    bool has_powerups;
    bool has_obstacles;
} Level;

typedef struct {
    float paddle_speed;      // Velocidade das raquetes (ambos)
    float ball_speed;        // Velocidade da bola
    bool powers_enabled;     // Poderes ativados
    int points_to_win;       // Pontos para vencer
    int selected_config_item; // Item selecionado no menu de config
    
    EnemyPowerType player1_power;
    EnemyPowerType player2_power;
    float player1_power_cooldown;
    float player2_power_cooldown;
    bool player1_power_active;
    bool player2_power_active;
    float player1_power_duration;
    float player2_power_duration;
    
    int player1_score;
    int player2_score;
    
    int winner; // 1 ou 2, 0 se não há vencedor ainda
} PvPConfig;

typedef struct {
    GameState current_state;
    GameState previous_state;

    Ball ball;
    Paddle player_paddle;
    Paddle ai_paddle;
    PowerUp powerups[5];

    VisualEffectsSystem visual_effects;

    Level levels[MAX_LEVELS];
    int current_level;

    int selected_menu_item;
    bool keys[256];

    bool in_options_menu;
    int selected_option_item;
    bool sound_enabled;
    int ai_difficulty; // 0=Fácil, 1=Médio, 2=Difícil
    float game_speed; // NOVO: 1.0f, 2.0f, 4.0f
    
    LevelDialog level_dialog;
    bool waiting_for_dialog;
    
    float game_timer;
    int combo_meter;
    bool frenzy_mode;
    float frenzy_timer;

    float paddle_sensitivity;
    bool effects_enabled;

    int total_hits;
    int consecutive_hits;
    int coins_earned;

    FileConfig* config;
    Enemy enemy;
    
    CheatSystem cheat_system;
    
    PvPConfig pvp_config;
} Game;

void game_init(Game* game);
void game_cleanup(Game* game);
void game_update(Game* game, float delta_time);
void game_render(Game* game);
void game_handle_input(Game* game);

void state_splash_update(Game* game, float delta_time);
void state_main_menu_update(Game* game, float delta_time);
void state_level_select_update(Game* game, float delta_time);
void state_gameplay_update(Game* game, float delta_time);
void state_pause_update(Game* game, float delta_time);
void state_victory_update(Game* game, float delta_time);
void state_defeat_update(Game* game, float delta_time);

void paddle_update_ai(Paddle* ai_paddle, Ball* ball, int difficulty, float delta_time);
void check_collisions(Game* game);
void update_powerups(Game* game, float delta_time);

void load_game_config(Game* game);
void save_game_config(Game* game);
void init_levels(Game* game);
float get_time_seconds(void);

void game_handle_input_callback(SDL_Event* event);
void register_game_states(Game* game);
void render_gameplay_state(Game* game, SDL_Renderer* renderer);
void render_splash_state(Game* game, SDL_Renderer* renderer);
void render_menu_state(Game* game, SDL_Renderer* renderer);
void render_level_select_state(Game* game, SDL_Renderer* renderer);
void render_pause_state(Game* game, SDL_Renderer* renderer);
void render_victory_state(Game* game, SDL_Renderer* renderer);
void render_defeat_state(Game* game, SDL_Renderer* renderer);

void game_handle_sdl_input(Game* game, SDL_Event* event);
void game_render_sdl(Game* game, SDL_Renderer* renderer);
void render_gameplay_state_sdl(Game* game, SDL_Renderer* renderer);
void render_splash_state_sdl(Game* game, SDL_Renderer* renderer);
void render_menu_state_sdl(Game* game, SDL_Renderer* renderer);
void render_level_select_state_sdl(Game* game, SDL_Renderer* renderer);
void render_pause_state_sdl(Game* game, SDL_Renderer* renderer);
void render_victory_state_sdl(Game* game, SDL_Renderer* renderer);
void render_defeat_state_sdl(Game* game, SDL_Renderer* renderer);


void menu_toggle_sound(Game* game);
void menu_cycle_difficulty(Game* game, int direction);
void menu_select_next_level(Game* game);
bool levels_is_completed(Game* game, int level_id);
void unlock_next_level(Game* game, int level_id);

static void setup_enemy_for_level(Game* game);

void render_enemy_power_effects(SDL_Renderer* renderer, Enemy* enemy);

void enemy_update_powers(Enemy* enemy, Ball* ball, Paddle* player_paddle, float delta_time);
void enemy_activate_power(Enemy* enemy, Ball* ball, Paddle* player_paddle);
void enemy_update_active_power(Enemy* enemy, Ball* ball, Paddle* player_paddle, float delta_time);
void enemy_activate_erro_bobo(Enemy* enemy, Ball* ball);
void enemy_activate_muralha_aco(Enemy* enemy);
void enemy_activate_bolada_atordoante(Enemy* enemy, Ball* ball, Paddle* player_paddle);
void enemy_activate_troca_percurso(Enemy* enemy, Ball* ball);
void enemy_activate_neblina_densa(Enemy* enemy);
void enemy_activate_choque_paralisante(Enemy* enemy, Paddle* player_paddle);
void enemy_activate_bola_caotica(Enemy* enemy, Ball* ball);
void enemy_activate_clone_sombras(Enemy* enemy);
void enemy_activate_dominio_absoluto(Enemy* enemy, Ball* ball, Paddle* player_paddle);
void enemy_update_muralha_aco(Enemy* enemy, Ball* ball, float delta_time);
void enemy_update_neblina_densa(Enemy* enemy, float delta_time);
void enemy_update_bola_caotica(Enemy* enemy, Ball* ball, float delta_time);
void enemy_update_clone_sombras(Enemy* enemy, float delta_time);
void enemy_update_dominio_absoluto(Enemy* enemy, Ball* ball, Paddle* player_paddle, float delta_time);
void enemy_update_visual_effects(Enemy* enemy, float delta_time);

void menu_cycle_game_speed(Game* game, int direction);
void start_level_dialog(Game* game);
void start_gameplay(Game* game);


void pvp_config_init(PvPConfig* config);
void pvp_generate_random_powers(PvPConfig* config);
void pvp_update_powers(Game* game, float delta_time);
void check_pvp_collisions(Game* game);

void state_pvp_config_update(Game* game, float delta_time);
void state_pvp_gameplay_update(Game* game, float delta_time);
void state_pvp_victory_update(Game* game, float delta_time);

void render_pvp_config_state_sdl(Game* game, SDL_Renderer* renderer);
void render_pvp_gameplay_state_sdl(Game* game, SDL_Renderer* renderer);
void render_pvp_victory_state_sdl(Game* game, SDL_Renderer* renderer);


int main(void) {
    printf("=== PINGOTECA - Viciante Ping Pong Game ===\n");
    printf("Inicializando sistema...\n");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "PINGOTECA",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        printf("Erro ao criar renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!text_renderer_init(renderer)) {
        printf("Erro ao inicializar sistema de texto\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (!audio_init()) {
        printf("Erro ao inicializar sistema de áudio\n");
    }

    Game game;
    game_init(&game);

    float last_time = SDL_GetTicks() / 1000.0f;
    bool running = true;

    printf("Entrando no loop principal do jogo...\n");

    while (running && game.current_state != STATE_EXIT) {
        float current_time = SDL_GetTicks() / 1000.0f;
        float delta_time = current_time - last_time;
        last_time = current_time;

        if (delta_time > 1.0f / 30.0f) {
            delta_time = 1.0f / 30.0f;
        }

        delta_time *= game.game_speed;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            game_handle_sdl_input(&game, &event);
        }

        game_update(&game, delta_time);
        game_render_sdl(&game, renderer);

        SDL_Delay(16); // ~60 FPS
    }

    printf("Encerrando jogo...\n");
    
    audio_cleanup(); // NOVO
    text_renderer_cleanup();
    game_cleanup(&game);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void game_handle_sdl_input(Game* game, SDL_Event* event) {
    if (!game) return;

    switch (game->current_state) {
        case STATE_MAIN_MENU:
            cheat_system_set_context(&game->cheat_system, CHEAT_CONTEXT_MENU);
            break;
        case STATE_LEVEL_SELECT:
            cheat_system_set_context(&game->cheat_system, CHEAT_CONTEXT_LEVEL_SELECT);
            break;
        case STATE_GAMEPLAY:
            cheat_system_set_context(&game->cheat_system, CHEAT_CONTEXT_GAMEPLAY);
            break;
        default:
            cheat_system_set_context(&game->cheat_system, CHEAT_CONTEXT_MENU);
            break;
    }

    if (cheat_system_is_interface_open(&game->cheat_system)) {
        cheat_system_handle_input(&game->cheat_system, event);
        return; // Não processar outros inputs
    }

    switch (event->type) {
        case SDL_KEYDOWN:
            if (event->key.keysym.sym == SDLK_j && 
                (event->key.keysym.mod & KMOD_SHIFT)) {
                cheat_system_toggle_interface(&game->cheat_system);
                return;
            }
            switch (event->key.keysym.sym) {
                case SDLK_w:
                    if (game->current_state == STATE_GAMEPLAY) {
                        game->keys[0] = true; // UP para jogador
                    }
                    else if (game->current_state == STATE_PVP_GAMEPLAY) {
                        game->keys[0] = true; // UP para Player 1 no PvP
                    }
                    else if (game->current_state == STATE_MAIN_MENU && !game->in_options_menu) {
                        audio_play_sound(SOUND_NAVIGATION);
                        game->selected_menu_item--;
                        if (game->selected_menu_item < 0) {
                            game->selected_menu_item = 3;
                        }
                        printf("Menu item selecionado: %d\n", game->selected_menu_item);
                    }
                    else if (game->current_state == STATE_MAIN_MENU && game->in_options_menu) {
                        audio_play_sound(SOUND_NAVIGATION);
                        game->selected_option_item--;
                        if (game->selected_option_item < 0) {
                            game->selected_option_item = 3;
                        }
                        printf("Opção selecionada: %d\n", game->selected_option_item);
                    }
                    else if (game->current_state == STATE_LEVEL_SELECT) {
                        audio_play_sound(SOUND_NAVIGATION);
                        game->current_level -= 3;
                        if (game->current_level < 0) {
                            game->current_level += 9;
                        }
                        if (game->current_level >= MAX_LEVELS) {
                            game->current_level = MAX_LEVELS - 1;
                        }
                        printf("Nível selecionado: %d\n", game->current_level);
                    }
                    else if (game->current_state == STATE_PVP_CONFIG) {
                        audio_play_sound(SOUND_NAVIGATION);
                        game->pvp_config.selected_config_item--;
                        if (game->pvp_config.selected_config_item < 0) {
                            game->pvp_config.selected_config_item = 5;
                        }
                        printf("Config PvP item selecionado: %d\n", game->pvp_config.selected_config_item);
                    }
                    break;
                    
                case SDLK_s:
                    if (game->current_state == STATE_GAMEPLAY) {
                        game->keys[1] = true; // DOWN para jogador
                    }
                    else if (game->current_state == STATE_PVP_GAMEPLAY) {
                        game->keys[1] = true; // DOWN para Player 1 no PvP
                    }
                    else if (game->current_state == STATE_MAIN_MENU && !game->in_options_menu) {
                        audio_play_sound(SOUND_NAVIGATION);
                        game->selected_menu_item++;
                        if (game->selected_menu_item > 3) {
                            game->selected_menu_item = 0;
                        }
                        printf("Menu item selecionado: %d\n", game->selected_menu_item);
                    }
                    else if (game->current_state == STATE_MAIN_MENU && game->in_options_menu) {
                        audio_play_sound(SOUND_NAVIGATION);
                        game->selected_option_item++;
                        if (game->selected_option_item > 3) {
                            game->selected_option_item = 0;
                        }
                        printf("Opção selecionada: %d\n", game->selected_option_item);
                    }
                    else if (game->current_state == STATE_LEVEL_SELECT) {
                        audio_play_sound(SOUND_NAVIGATION);
                        game->current_level += 3;
                        if (game->current_level >= MAX_LEVELS) {
                            game->current_level = game->current_level % MAX_LEVELS;
                        }
                        printf("Nível selecionado: %d\n", game->current_level);
                    }
                    else if (game->current_state == STATE_PVP_CONFIG) {
                        audio_play_sound(SOUND_NAVIGATION);
                        game->pvp_config.selected_config_item++;
                        if (game->pvp_config.selected_config_item > 5) {
                            game->pvp_config.selected_config_item = 0;
                        }
                        printf("Config PvP item selecionado: %d\n", game->pvp_config.selected_config_item);
                    }
                    break;
                    
                case SDLK_UP:
                    if (game->current_state == STATE_PVP_GAMEPLAY) {
                        game->keys[2] = true; // UP para Player 2 no PvP
                    }
                    else if (game->current_state == STATE_GAMEPLAY) {
                        if (game->cheat_system.ball_control) {
                            game->keys[2] = true; // Novo índice para controle da bola UP
                        } else {
                            game->keys[0] = true; // Controle normal do jogador
                        }
                    }
                    else {
                        audio_play_sound(SOUND_NAVIGATION);
                        if (game->current_state == STATE_MAIN_MENU && !game->in_options_menu) {
                            game->selected_menu_item--;
                            if (game->selected_menu_item < 0) {
                                game->selected_menu_item = 3;
                            }
                        }
                        else if (game->current_state == STATE_MAIN_MENU && game->in_options_menu) {
                            game->selected_option_item--;
                            if (game->selected_option_item < 0) {
                                game->selected_option_item = 3;
                            }
                        }
                        else if (game->current_state == STATE_LEVEL_SELECT) {
                            game->current_level -= 3;
                            if (game->current_level < 0) {
                                game->current_level += 9;
                            }
                            if (game->current_level >= MAX_LEVELS) {
                                game->current_level = MAX_LEVELS - 1;
                            }
                        }
                    }
                    break;
                    
                case SDLK_DOWN:
                    if (game->current_state == STATE_PVP_GAMEPLAY) {
                        game->keys[3] = true; // DOWN para Player 2 no PvP
                    }
                    else if (game->current_state == STATE_GAMEPLAY) {
                        if (game->cheat_system.ball_control) {
                            game->keys[3] = true; // Novo índice para controle da bola DOWN
                        } else {
                            game->keys[1] = true; // Controle normal do jogador
                        }
                    }
                    else {
                        audio_play_sound(SOUND_NAVIGATION);
                        if (game->current_state == STATE_MAIN_MENU && !game->in_options_menu) {
                            game->selected_menu_item++;
                            if (game->selected_menu_item > 3) {
                                game->selected_menu_item = 0;
                            }
                        }
                        else if (game->current_state == STATE_MAIN_MENU && game->in_options_menu) {
                            game->selected_option_item++;
                            if (game->selected_option_item > 3) {
                                game->selected_option_item = 0;
                            }
                        }
                        else if (game->current_state == STATE_LEVEL_SELECT) {
                            game->current_level += 3;
                            if (game->current_level >= MAX_LEVELS) {
                                game->current_level = game->current_level % MAX_LEVELS;
                            }
                        }
                    }
                    break;

                case SDLK_a:
                case SDLK_LEFT:
                    if (game->current_state == STATE_LEVEL_SELECT) {
                        audio_play_sound(SOUND_NAVIGATION);
                        game->current_level--;
                        if (game->current_level < 0) {
                            game->current_level = MAX_LEVELS - 1;
                        }
                        printf("Nível selecionado: %d\n", game->current_level);
                    }
                    else if (game->current_state == STATE_MAIN_MENU && game->in_options_menu) {
                        audio_play_sound(SOUND_NAVIGATION);
                        if (game->selected_option_item == 0) {
                            menu_toggle_sound(game);
                        } else if (game->selected_option_item == 1) {
                            menu_cycle_difficulty(game, -1);
                        } else if (game->selected_option_item == 2) {
                            menu_cycle_game_speed(game, -1);
                        }
                    }
                    else if (game->current_state == STATE_PVP_CONFIG) {
                        audio_play_sound(SOUND_NAVIGATION);
                        switch (game->pvp_config.selected_config_item) {
                            case 0: // Velocidade raquetes
                                game->pvp_config.paddle_speed -= 50.0f;
                                if (game->pvp_config.paddle_speed < 100.0f) {
                                    game->pvp_config.paddle_speed = 100.0f;
                                }
                                break;
                            case 1: // Velocidade bola
                                game->pvp_config.ball_speed -= 50.0f;
                                if (game->pvp_config.ball_speed < 150.0f) {
                                    game->pvp_config.ball_speed = 150.0f;
                                }
                                break;
                            case 2: // Poderes
                                game->pvp_config.powers_enabled = !game->pvp_config.powers_enabled;
                                if (game->pvp_config.powers_enabled) {
                                    pvp_generate_random_powers(&game->pvp_config);
                                }
                                break;
                            case 3: // Pontos para vencer
                                game->pvp_config.points_to_win--;
                                if (game->pvp_config.points_to_win < 1) {
                                    game->pvp_config.points_to_win = 1;
                                }
                                break;
                        }
                    }
                    break;
                    
                case SDLK_d:
                case SDLK_RIGHT:
                    if (game->current_state == STATE_LEVEL_SELECT) {
                        audio_play_sound(SOUND_NAVIGATION);
                        game->current_level++;
                        if (game->current_level >= MAX_LEVELS) {
                            game->current_level = 0;
                        }
                        printf("Nível selecionado: %d\n", game->current_level);
                    }
                    else if (game->current_state == STATE_MAIN_MENU && game->in_options_menu) {
                        audio_play_sound(SOUND_NAVIGATION);
                        if (game->selected_option_item == 0) {
                            menu_toggle_sound(game);
                        } else if (game->selected_option_item == 1) {
                            menu_cycle_difficulty(game, 1);
                        } else if (game->selected_option_item == 2) {
                            menu_cycle_game_speed(game, 1);
                        }
                    }
                    else if (game->current_state == STATE_PVP_CONFIG) {
                        audio_play_sound(SOUND_NAVIGATION);
                        switch (game->pvp_config.selected_config_item) {
                            case 0: // Velocidade raquetes
                                game->pvp_config.paddle_speed += 50.0f;
                                if (game->pvp_config.paddle_speed > 600.0f) {
                                    game->pvp_config.paddle_speed = 600.0f;
                                }
                                break;
                            case 1: // Velocidade bola
                                game->pvp_config.ball_speed += 50.0f;
                                if (game->pvp_config.ball_speed > 600.0f) {
                                    game->pvp_config.ball_speed = 600.0f;
                                }
                                break;
                            case 2: // Poderes
                                game->pvp_config.powers_enabled = !game->pvp_config.powers_enabled;
                                if (game->pvp_config.powers_enabled) {
                                    pvp_generate_random_powers(&game->pvp_config);
                                }
                                break;
                            case 3: // Pontos para vencer
                                game->pvp_config.points_to_win++;
                                if (game->pvp_config.points_to_win > 15) {
                                    game->pvp_config.points_to_win = 15;
                                }
                                break;
                        }
                    }
                    break;
                    
                case SDLK_SPACE:
                case SDLK_RETURN:
                    audio_play_sound(SOUND_SELECT);
                    
                    if (game->current_state == STATE_GAMEPLAY && game->waiting_for_dialog) {
                        if (dialog_is_waiting_for_input(&game->level_dialog)) {
                            dialog_advance(&game->level_dialog);
                            
                            if (dialog_is_finished(&game->level_dialog)) {
                                game->waiting_for_dialog = false;
                                printf("Todos os diálogos finalizados, iniciando gameplay...\n");
                            }
                        }
                        break;
                    }
    
                    if (game->current_state == STATE_SPLASH) {
                        game->current_state = STATE_MAIN_MENU;
                        game->selected_menu_item = 0;
                        printf("TRANSIÇÃO: Splash -> Main Menu\n");
                    } 
                    else if (game->current_state == STATE_MAIN_MENU && !game->in_options_menu) {
                        switch (game->selected_menu_item) {
                            case 0:
                                game->current_state = STATE_LEVEL_SELECT;
                                printf("TRANSIÇÃO: Main Menu -> Level Select\n");
                                break;
                            case 1: // NOVO: PvP
                                game->current_state = STATE_PVP_CONFIG;
                                game->pvp_config.selected_config_item = 0;
                                printf("TRANSIÇÃO: Main Menu -> PvP Config\n");
                                break;
                            case 2: // MODIFICADO: agora é posição 2
                                game->in_options_menu = true;
                                game->selected_option_item = 0;
                                printf("TRANSIÇÃO: Main Menu -> Options\n");
                                break;
                            case 3: // MODIFICADO: agora é posição 3
                                game->current_state = STATE_EXIT;
                                printf("TRANSIÇÃO: Main Menu -> Exit\n");
                                break;
                        }
                    }
                    else if (game->current_state == STATE_MAIN_MENU && game->in_options_menu) {
                        switch (game->selected_option_item) {
                            case 0:
                                menu_toggle_sound(game);
                                break;
                            case 1:
                                menu_cycle_difficulty(game, 1);
                                break;
                            case 2:
                                menu_cycle_game_speed(game, 1);
                                break;
                            case 3:
                                menu_select_next_level(game);
                                break;
                        }
                    }
                    else if (game->current_state == STATE_LEVEL_SELECT) {
                        if (game->levels[game->current_level].unlocked) {
                            start_gameplay(game);
                            
                            if (game->current_level == 0 || game->current_level == 4 || game->current_level == 8) {
                                start_level_dialog(game);
                            }
                            
                            printf("TRANSIÇÃO: Level Select -> Gameplay (Nível %d)\n", game->current_level);
                        } else {
                            printf("Nível %d não está desbloqueado!\n", game->current_level);
                        }
                    }
                    else if (game->current_state == STATE_VICTORY || game->current_state == STATE_GAME_OVER) {
                        game->current_state = STATE_LEVEL_SELECT;
                        printf("TRANSIÇÃO: Victory/Game Over -> Level Select\n");
                    }
                    else if (game->current_state == STATE_PVP_CONFIG) {
                        audio_play_sound(SOUND_SELECT);
                        
                        switch (game->pvp_config.selected_config_item) {
                            case 4: // JOGAR
                                game->current_state = STATE_PVP_GAMEPLAY;
                                
                                game->pvp_config.player1_score = 0;
                                game->pvp_config.player2_score = 0;
                                game->pvp_config.winner = 0;
                                
                                game->player_paddle.speed = game->pvp_config.paddle_speed;
                                game->ai_paddle.speed = game->pvp_config.paddle_speed;
                                
                                ball_init(&game->ball, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
                                
                                game->ball.vx = (rand() % 2 == 0) ? game->pvp_config.ball_speed : -game->pvp_config.ball_speed;
                                
                                printf("TRANSIÇÃO: PvP Config -> PvP Gameplay\n");
                                break;
                                
                            case 5: // VOLTAR
                                game->current_state = STATE_MAIN_MENU;
                                printf("TRANSIÇÃO: PvP Config -> Main Menu\n");
                                break;
                        }
                    }
                    break;

                case SDLK_ESCAPE:
                    if (game->current_state == STATE_MAIN_MENU && game->in_options_menu) {
                        game->in_options_menu = false;
                        printf("TRANSIÇÃO: Options -> Main Menu\n");
                    }
                    else if (game->current_state == STATE_GAMEPLAY || game->current_state == STATE_PAUSE) {
                        game->current_state = STATE_LEVEL_SELECT;
                        audio_play_music(MUSIC_MENU);
                        printf("TRANSIÇÃO: Gameplay/Pause -> Level Select\n");
                    }
                    else if (game->current_state == STATE_LEVEL_SELECT) {
                        game->current_state = STATE_MAIN_MENU;
                        audio_play_music(MUSIC_MENU);
                        printf("TRANSIÇÃO: Level Select -> Main Menu\n");
                    }
                    else if (game->current_state == STATE_PVP_CONFIG) {
                        game->current_state = STATE_MAIN_MENU;
                        printf("TRANSIÇÃO: PvP Config -> Main Menu\n");
                    }
                    else if (game->current_state == STATE_PVP_GAMEPLAY) {
                        game->current_state = STATE_PVP_CONFIG;
                        printf("TRANSIÇÃO: PvP Gameplay -> PvP Config\n");
                    }
                    else if (game->current_state == STATE_PVP_VICTORY) {
                        game->current_state = STATE_PVP_CONFIG;
                        game->pvp_config.player1_score = 0;
                        game->pvp_config.player2_score = 0;
                        game->pvp_config.winner = 0;
                        printf("TRANSIÇÃO: PvP Victory -> PvP Config\n");
                    }
                    else if (game->current_state == STATE_MAIN_MENU) {
                        game->current_state = STATE_EXIT;
                        printf("TRANSIÇÃO: Main Menu -> Exit\n");
                    }
                    else {
                        game->current_state = STATE_EXIT;
                    }
                    break;
                    
                case SDLK_p:
                    if (game->current_state == STATE_GAMEPLAY) {
                        game->previous_state = game->current_state;
                        game->current_state = STATE_PAUSE;
                        printf("TRANSIÇÃO: Gameplay -> Pause\n");
                    } else if (game->current_state == STATE_PAUSE) {
                        game->current_state = game->previous_state;
                        printf("TRANSIÇÃO: Pause -> Gameplay\n");
                    }
                    break;
            }
            break;
            
        case SDL_KEYUP:
            switch (event->key.keysym.sym) {
                case SDLK_w:
                    if (game->current_state == STATE_GAMEPLAY) {
                        game->keys[0] = false;
                    }
                    else if (game->current_state == STATE_PVP_GAMEPLAY) {
                        game->keys[0] = false; // Player 1 UP no PvP
                    }
                    break;
                case SDLK_s:
                    if (game->current_state == STATE_GAMEPLAY) {
                        game->keys[1] = false;
                    }
                    else if (game->current_state == STATE_PVP_GAMEPLAY) {
                        game->keys[1] = false; // Player 1 DOWN no PvP
                    }
                    break;
                case SDLK_UP:
                    if (game->current_state == STATE_PVP_GAMEPLAY) {
                        game->keys[2] = false; // Player 2 UP no PvP
                    }
                    else if (game->current_state == STATE_GAMEPLAY) {
                        if (game->cheat_system.ball_control) {
                            game->keys[2] = false; // Ball control UP
                        } else {
                            game->keys[0] = false; // Player UP
                        }
                    }
                    break;
                case SDLK_DOWN:
                    if (game->current_state == STATE_PVP_GAMEPLAY) {
                        game->keys[3] = false; // Player 2 DOWN no PvP
                    }
                    else if (game->current_state == STATE_GAMEPLAY) {
                        if (game->cheat_system.ball_control) {
                            game->keys[3] = false; // Ball control DOWN
                        } else {
                            game->keys[1] = false; // Player DOWN
                        }
                    }
                    break;
            }
            break;
    }
}

void game_handle_input_callback(SDL_Event* event) {
    static Game* current_game = NULL;
    
    if (!current_game) {
        return;
    }

    switch (event->type) {
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_w:
                case SDLK_UP:
                    current_game->keys[SDLK_UP] = true;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    current_game->keys[SDLK_DOWN] = true;
                    break;
                case SDLK_SPACE:
                    if (current_game->current_state == STATE_SPLASH) {
                        current_game->current_state = STATE_MAIN_MENU;
                        printf("TRANSIÇÃO: Splash -> Main Menu\n");
                    } else if (current_game->current_state == STATE_MAIN_MENU) {
                        current_game->current_state = STATE_LEVEL_SELECT;
                        printf("TRANSIÇÃO: Main Menu -> Level Select\n");
                    } else if (current_game->current_state == STATE_LEVEL_SELECT) {
                        current_game->current_level = 0;
                        current_game->current_state = STATE_GAMEPLAY;
                        ball_init(&current_game->ball, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
                        printf("TRANSIÇÃO: Level Select -> Gameplay\n");
                    }
                    break;
                case SDLK_p:
                    if (current_game->current_state == STATE_GAMEPLAY) {
                        current_game->previous_state = current_game->current_state;
                        current_game->current_state = STATE_PAUSE;
                        printf("TRANSIÇÃO: Gameplay -> Pause\n");
                    } else if (current_game->current_state == STATE_PAUSE) {
                        current_game->current_state = current_game->previous_state;
                        printf("TRANSIÇÃO: Pause -> Gameplay\n");
                    }
                    break;
            }
            break;
            
        case SDL_KEYUP:
            switch (event->key.keysym.sym) {
                case SDLK_w:
                case SDLK_UP:
                    current_game->keys[SDLK_UP] = false;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    current_game->keys[SDLK_DOWN] = false;
                    break;
            }
            break;
    }
}

void menu_cycle_game_speed(Game* game, int direction) {
    if (!game) return;
    
    if (direction > 0) {
        if (game->game_speed == 1.0f) {
            game->game_speed = 2.0f;
        } else if (game->game_speed == 2.0f) {
            game->game_speed = 4.0f;
        } else {
            game->game_speed = 1.0f;
        }
    } else {
        if (game->game_speed == 4.0f) {
            game->game_speed = 2.0f;
        } else if (game->game_speed == 2.0f) {
            game->game_speed = 1.0f;
        } else {
            game->game_speed = 4.0f;
        }
    }
    
    printf("Velocidade do jogo: %.0fx\n", game->game_speed);
}

void start_level_dialog(Game* game) {
    char enemy_name[32];
    if (game->current_level == 0) {
        snprintf(enemy_name, sizeof(enemy_name), "inimigo1");
    } else if (game->current_level == 4) {
        snprintf(enemy_name, sizeof(enemy_name), "inimigo5");
    } else if (game->current_level == 8) {
        snprintf(enemy_name, sizeof(enemy_name), "inimigo9");
    }
    
    if (game->current_level == 0) {
        const char* tutorial_texts[] = {
            "BEM-VINDO AO PINGOTECA! O jogo de ping pong mais viciante que você já jogou!",
            "Olá, novato! Prepare-se para ser derrotado!",
            "Use as teclas W e S ou as setas para mover sua raquete. Vou facilitar para você...",
            "Não subestime minha habilidade, humano!"
        };
        dialog_start_multiple(&game->level_dialog, tutorial_texts, 4, 0.0f);
        
        dialog_set_speaker(&game->level_dialog, "jogador", false);
        
    } else if (game->current_level == 4) {
        const char* powers_texts[] = {
            "ATENÇÃO! A partir deste nível, seus adversários possuem poderes especiais!",
            "Finalmente chegou ao meu território! Prepare-se para meus poderes!",
            "Observe bem os efeitos visuais - eles indicam quando um poder está sendo usado.",
            "Você não tem chances contra minha magia!"
        };
        dialog_start_multiple(&game->level_dialog, powers_texts, 4, 0.0f);
        
        dialog_set_speaker(&game->level_dialog, "jogador", false);
        
    } else if (game->current_level == 8) {
        const char* final_texts[] = {
            "ESTE É O MOMENTO FINAL! Você chegou ao último desafio!",
            "Impressionante... poucos chegaram até aqui. Sou o MESTRE DOS PINGOS!",
            "Sua jornada foi longa, mas aqui ela termina.",
            "Domino todos os poderes. Prepare-se para o confronto definitivo!",
            "QUE A BATALHA FINAL COMECE!"
        };
        dialog_start_multiple(&game->level_dialog, final_texts, 5, 0.0f);
        
        dialog_set_speaker(&game->level_dialog, "jogador", false);
    }
    
    game->waiting_for_dialog = true;
}

void start_gameplay(Game* game) {
    game->current_state = STATE_GAMEPLAY;
    game->player_paddle.score = 0;
    game->ai_paddle.score = 0;
    ball_init(&game->ball, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    setup_enemy_for_level(game);
    
    if (game->current_level >= 5) { // Níveis 6-9 são boss fights
        audio_play_music(MUSIC_BOSS);
    } else {
        audio_play_music(MUSIC_GAMEPLAY);
    }
}

void game_init(Game* game) {
    memset(game, 0, sizeof(Game));

    game->current_state = STATE_SPLASH;
    game->previous_state = STATE_SPLASH;

    game->in_options_menu = false;
    game->selected_option_item = 0;
    game->sound_enabled = true;
    game->ai_difficulty = 1; // Médio por padrão

    game->config = fileconfig_create();
    load_game_config(game);

    ball_init(&game->ball, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    game->player_paddle.x = 50;
    game->player_paddle.y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    game->player_paddle.width = PADDLE_WIDTH;
    game->player_paddle.height = PADDLE_HEIGHT;
    game->player_paddle.speed = 300;
    game->player_paddle.score = 0;

    game->ai_paddle.x = SCREEN_WIDTH - 50 - PADDLE_WIDTH;
    game->ai_paddle.y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    game->ai_paddle.width = PADDLE_WIDTH;
    game->ai_paddle.height = PADDLE_HEIGHT;
    game->ai_paddle.speed = 250;
    game->ai_paddle.score = 0;

    visual_effects_init(&game->visual_effects);
    
    enemy_init(&game->enemy, ENEMY_BEGINNER, 1);
    
    pvp_config_init(&game->pvp_config);

    init_levels(game);

    game->paddle_sensitivity = 1.0f;
    game->effects_enabled = true;
    
    cheat_system_init(&game->cheat_system);

    printf("Jogo inicializado com sucesso!\n");
}

static void setup_enemy_for_level(Game* game) {
    const LevelData* level_data = &LEVELS_DATA[game->current_level];
    
    EnemyType enemy_type;
    switch (game->current_level) {
        case 0:
        case 1:
            enemy_type = ENEMY_BEGINNER;
            break;
        case 2:
            enemy_type = ENEMY_DEFENDER;
            break;
        case 3:
            enemy_type = ENEMY_AGGRESSOR;
            break;
        case 4:
            enemy_type = ENEMY_ADAPTIVE;
            break;
        case 5:
            enemy_type = ENEMY_BOSS_STORM;
            break;
        case 6:
            enemy_type = ENEMY_BOSS_LIGHTNING;
            break;
        case 7:
            enemy_type = ENEMY_BOSS_CHAOS;
            break;
        case 8:
            enemy_type = ENEMY_BOSS_MASTER;
            break;
        default:
            enemy_type = ENEMY_BEGINNER;
            break;
    }
    
    enemy_init(&game->enemy, enemy_type, game->current_level + 1);
    
    game->enemy.paddle.x = game->ai_paddle.x;
    game->enemy.paddle.y = game->ai_paddle.y;
    game->enemy.paddle.width = game->ai_paddle.width;
    game->enemy.paddle.height = game->ai_paddle.height;
    game->enemy.paddle.speed = game->ai_paddle.speed;
    
    printf("Inimigo configurado para nível %d: %s\n", game->current_level + 1, game->enemy.name);
}

void menu_toggle_sound(Game* game) {
    if (!game) return;
    
    game->sound_enabled = !game->sound_enabled;
    audio_set_enabled(game->sound_enabled);
    printf("Som %s\n", game->sound_enabled ? "LIGADO" : "DESLIGADO");
}

void menu_cycle_difficulty(Game* game, int direction) {
    if (!game) return;
    
    game->ai_difficulty += direction;
    
    if (game->ai_difficulty < 0) {
        game->ai_difficulty = 2; // Volta para Difícil
    } else if (game->ai_difficulty > 2) {
        game->ai_difficulty = 0; // Volta para Fácil
    }
    
    const char* difficulty_names[] = {"FÁCIL", "MÉDIO", "DIFÍCIL"};
    printf("Dificuldade IA: %s\n", difficulty_names[game->ai_difficulty]);
}

void menu_select_next_level(Game* game) {
    if (!game) return;
    
    if (!levels_is_completed(game, game->current_level)) {
        printf("Nível atual não foi completado ainda!\n");
        return;
    }
    
    int next_level = game->current_level + 1;
    if (next_level >= MAX_LEVELS) {
        printf("Você já está no último nível!\n");
        return;
    }
    
    if (!game->levels[next_level].unlocked) {
        printf("Próximo nível não está desbloqueado!\n");
        return;
    }
    
    game->current_level = next_level;
    game->current_state = STATE_GAMEPLAY;
    game->in_options_menu = false;
    
    game->player_paddle.score = 0;
    game->ai_paddle.score = 0;
    
    ball_init(&game->ball, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    
    printf("Indo para nível %d\n", next_level + 1);
}

bool levels_is_completed(Game* game, int level_id) {
    if (!game || level_id < 0 || level_id >= MAX_LEVELS) return false;
    return game->levels[level_id].completed;
}

void unlock_next_level(Game* game, int level_id) {
    if (!game || level_id < 0 || level_id >= MAX_LEVELS - 1) return;
    
    game->levels[level_id + 1].unlocked = true;
    printf("Nível %d desbloqueado!\n", level_id + 2);
}

void register_game_states(Game* game) {
    printf("Estados do jogo registrados.\n");
}

void game_cleanup(Game* game) {
    if (!game) return;

    save_game_config(game);
    fileconfig_destroy(game->config);

    printf("Recursos liberados.\n");
}

void game_update(Game* game, float delta_time) {
    if (!game) return;

    game->game_timer += delta_time;

    cheat_system_update(&game->cheat_system, delta_time);

    visual_effects_update(&game->visual_effects, delta_time);

    if (game->frenzy_mode) {
        game->frenzy_timer -= delta_time;
        if (game->frenzy_timer <= 0) {
            game->frenzy_mode = false;
            printf("Modo frenético desativado!\n");
        }
    }

    switch (game->current_state) {
        case STATE_SPLASH:
            state_splash_update(game, delta_time);
            break;
        case STATE_MAIN_MENU:
            state_main_menu_update(game, delta_time);
            break;
        case STATE_LEVEL_SELECT:
            state_level_select_update(game, delta_time);
            break;
        case STATE_GAMEPLAY:
            state_gameplay_update(game, delta_time);
            break;
        case STATE_PAUSE:
            state_pause_update(game, delta_time);
            break;
        case STATE_VICTORY:
            state_victory_update(game, delta_time);
            break;
        case STATE_GAME_OVER:
            state_defeat_update(game, delta_time);
            break;
        case STATE_PVP_CONFIG:
            state_pvp_config_update(game, delta_time);
            break;
        case STATE_PVP_GAMEPLAY:
            state_pvp_gameplay_update(game, delta_time);
            break;
        case STATE_PVP_VICTORY:
            state_pvp_victory_update(game, delta_time);
            break;
        default:
            break;
    }
}

void game_render_sdl(Game* game, SDL_Renderer* renderer) {
    if (!game || !renderer) return;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    switch (game->current_state) {
        case STATE_SPLASH:
            render_splash_state_sdl(game, renderer);
            break;
        case STATE_MAIN_MENU:
            render_menu_state_sdl(game, renderer);
            break;
        case STATE_LEVEL_SELECT:
            render_level_select_state_sdl(game, renderer);
            break;
        case STATE_GAMEPLAY:
            render_gameplay_state_sdl(game, renderer);
            break;
        case STATE_PAUSE:
            render_pause_state_sdl(game, renderer);
            break;
        case STATE_VICTORY:
            render_victory_state_sdl(game, renderer);
            break;
        case STATE_GAME_OVER:
            render_defeat_state_sdl(game, renderer);
            break;
        case STATE_PVP_CONFIG:
            render_pvp_config_state_sdl(game, renderer);
            break;
        case STATE_PVP_GAMEPLAY:
            render_pvp_gameplay_state_sdl(game, renderer);
            break;
        case STATE_PVP_VICTORY:
            render_pvp_victory_state_sdl(game, renderer);
            break;
    }

    if (game->effects_enabled) {
        visual_effects_render(&game->visual_effects);
    }

    SDL_RenderPresent(renderer);
}

void render_splash_state_sdl(Game* game, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        int color = 10 + (y * 40) / SCREEN_HEIGHT;
        SDL_SetRenderDrawColor(renderer, color/2, color/3, color, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }

    SDL_SetRenderDrawColor(renderer, 64, 128, 255, 255);
    SDL_Rect logo = {
        SCREEN_WIDTH / 2 - 150,
        SCREEN_HEIGHT / 2 - 100,
        300, 80
    };
    SDL_RenderFillRect(renderer, &logo);

    TextColor title_color = TEXT_COLOR_WHITE;
    text_render_centered("PINGOTECA", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 50, 300, TEXT_STYLE_TITLE, title_color);

    TextColor subtitle_color = TEXT_COLOR_YELLOW;
    text_render_centered("O PING PONG VICIANTE", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 10, 300, TEXT_STYLE_NORMAL, subtitle_color);

    TextColor instruction_color = TEXT_COLOR_GRAY;
    text_render_centered("PRESSIONE ESPACO PARA CONTINUAR", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT - 80, 400, TEXT_STYLE_SMALL, instruction_color);

    static float blink_timer = 0;
    blink_timer += 0.1f;
    if (sinf(blink_timer * 3.0f) > 0) {
        TextColor blink_color = TEXT_COLOR_WHITE;
        text_render_centered("PRESSIONE ESPACO PARA CONTINUAR", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT - 80, 400, TEXT_STYLE_SMALL, blink_color);
    }
}

void render_gameplay_state(Game* game, SDL_Renderer* renderer) {
    render_gameplay_state_sdl(game, renderer);
}

void render_menu_state_sdl(Game* game, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        int color = 20 + (y * 30) / SCREEN_HEIGHT;
        SDL_SetRenderDrawColor(renderer, color, color, color + 20, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }

    TextColor title_color = TEXT_COLOR_WHITE;
    text_render_centered("MENU PRINCIPAL", SCREEN_WIDTH / 2 - 150, 80, 300, TEXT_STYLE_TITLE, title_color);

    if (!game->in_options_menu) {
        const char* options[] = {"JOGAR", "PVP", "OPCOES", "SAIR"};
        const char* descriptions[] = {
            "MODO CAMPANHA",
            "JOGADOR VS JOGADOR",
            "CONFIGURAR JOGO",
            "SAIR DO JOGO"
        };

        int start_y = (SCREEN_HEIGHT / 2) - 80;

        for (int i = 0; i < 4; i++) { // MODIFICADO: agora são 4 opções
            SDL_Rect item = {
                SCREEN_WIDTH / 2 - 150,
                start_y + i * 80,
                300, 60
            };

            if (i == game->selected_menu_item) {
                SDL_SetRenderDrawColor(renderer, 64, 128, 255, 255);
                SDL_RenderFillRect(renderer, &item);
                
                SDL_SetRenderDrawColor(renderer, 128, 164, 255, 255);
                for (int border = 0; border < 3; border++) {
                    SDL_Rect border_rect = {
                        item.x - border, item.y - border,
                        item.w + 2 * border, item.h + 2 * border
                    };
                    SDL_RenderDrawRect(renderer, &border_rect);
                }

                TextColor selected_color = TEXT_COLOR_WHITE;
                text_render_centered(options[i], item.x, item.y + 15, item.w, TEXT_STYLE_BOLD, selected_color);

                TextColor desc_color = TEXT_COLOR_YELLOW;
                text_render_centered(descriptions[i], item.x, item.y + 35, item.w, TEXT_STYLE_SMALL, desc_color);
            } else {
                SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
                SDL_RenderFillRect(renderer, &item);

                SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
                SDL_RenderDrawRect(renderer, &item);

                TextColor normal_color = TEXT_COLOR_WHITE;
                text_render_centered(options[i], item.x, item.y + 22, item.w, TEXT_STYLE_NORMAL, normal_color);
            }
        }
    }else{
        TextColor subtitle_color = TEXT_COLOR_YELLOW;
        text_render_centered("OPCOES", SCREEN_WIDTH / 2 - 100, 160, 200, TEXT_STYLE_BOLD, subtitle_color);

        char option_texts[4][64];
        snprintf(option_texts[0], 64, "SOM: %s", game->sound_enabled ? "LIGADO" : "DESLIGADO");
        
        const char* difficulty_names[] = {"FACIL", "MEDIO", "DIFICIL"};
        snprintf(option_texts[1], 64, "DIFICULDADE IA: %s", difficulty_names[game->ai_difficulty]);
        
        snprintf(option_texts[2], 64, "VELOCIDADE: %.0fX", game->game_speed);
        
        bool can_advance = levels_is_completed(game, game->current_level) && 
                          (game->current_level + 1 < MAX_LEVELS) && 
                          game->levels[game->current_level + 1].unlocked;
        snprintf(option_texts[3], 64, "PROXIMO NIVEL: %s", can_advance ? "DISPONIVEL" : "BLOQUEADO");

        for (int i = 0; i < 4; i++) {
            SDL_Rect item = {
                SCREEN_WIDTH / 2 - 200,
                220 + i * 80,
                400, 70
            };

            if (i == game->selected_option_item) {
                SDL_SetRenderDrawColor(renderer, 64, 128, 255, 255);
                SDL_RenderFillRect(renderer, &item);
                
                SDL_SetRenderDrawColor(renderer, 128, 164, 255, 255);
                for (int border = 0; border < 3; border++) {
                    SDL_Rect border_rect = {
                        item.x - border, item.y - border,
                        item.w + 2 * border, item.h + 2 * border
                    };
                    SDL_RenderDrawRect(renderer, &border_rect);
                }

                TextColor selected_color = TEXT_COLOR_WHITE;
                text_render_centered(option_texts[i], item.x, item.y + 20, item.w, TEXT_STYLE_BOLD, selected_color);

                if (i == 0 || i == 1 || i == 2) {
                    TextColor hint_color = TEXT_COLOR_YELLOW;
                    text_render_centered("USE SETAS ESQUERDA/DIREITA", item.x, item.y + 45, item.w, TEXT_STYLE_SMALL, hint_color);
                }
            } else {
                if (i == 3 && !can_advance) {
                    SDL_SetRenderDrawColor(renderer, 60, 30, 30, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
                }
                SDL_RenderFillRect(renderer, &item);

                SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
                SDL_RenderDrawRect(renderer, &item);

                TextColor normal_color = (i == 3 && !can_advance) ? TEXT_COLOR_RED : TEXT_COLOR_WHITE;
                text_render_centered(option_texts[i], item.x, item.y + 25, item.w, TEXT_STYLE_NORMAL, normal_color);
            }
        }
        
        TextColor back_color = TEXT_COLOR_GRAY;
        text_render_centered("ESC PARA VOLTAR", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 80, 200, TEXT_STYLE_SMALL, back_color);
    }

    if (!game->in_options_menu) {
        TextColor nav_color = TEXT_COLOR_GRAY;
        text_render_centered("USE SETAS PARA NAVEGAR - ENTER PARA SELECIONAR", SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT - 60, 500, TEXT_STYLE_SMALL, nav_color);
        
    }

    cheat_system_render(&game->cheat_system, renderer);
}

void render_level_select_state_sdl(Game* game, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        int color = 30 + (y * 20) / SCREEN_HEIGHT;
        SDL_SetRenderDrawColor(renderer, color, color, color + 20, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }

    TextColor title_color = TEXT_COLOR_WHITE;
    text_render_centered("SELECAO DE NIVEL", SCREEN_WIDTH / 2 - 150, 40, 300, TEXT_STYLE_TITLE, title_color);

    char level_info[128];
    snprintf(level_info, sizeof(level_info), "NIVEL %d: %s", 
             game->current_level + 1, game->levels[game->current_level].name);
    TextColor info_color = TEXT_COLOR_YELLOW;
    text_render_centered(level_info, SCREEN_WIDTH / 2 - 200, 90, 400, TEXT_STYLE_NORMAL, info_color);

    int grid_start_x = (SCREEN_WIDTH - (3 * 150 + 2 * 20)) / 2; // Centralizar horizontalmente
    int grid_start_y = 140; // Posição Y inicial
    
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            int level_id = y * 3 + x;
            if (level_id >= MAX_LEVELS) break;

            SDL_Rect level = {
                grid_start_x + x * 170, // CORRIGIDO: espaçamento para 1024 de largura
                grid_start_y + y * 130, // CORRIGIDO: espaçamento para 768 de altura
                150, 110 // CORRIGIDO: tamanho maior
            };

            if (level_id == game->current_level) {
                SDL_SetRenderDrawColor(renderer, 255, 128, 64, 255);
                SDL_RenderFillRect(renderer, &level);
                
                static float pulse_timer = 0;
                pulse_timer += 0.1f;
                int pulse_intensity = (int)(128 + 64 * sinf(pulse_timer * 4.0f));
                SDL_SetRenderDrawColor(renderer, 255, pulse_intensity, 128, 255);
                for (int border = 0; border < 4; border++) {
                    SDL_Rect border_rect = {
                        level.x - border,
                        level.y - border,
                        level.w + 2 * border,
                        level.h + 2 * border
                    };
                    SDL_RenderDrawRect(renderer, &border_rect);
                }
            } else if (game->levels[level_id].unlocked) {
                if (game->levels[level_id].completed) {
                    SDL_SetRenderDrawColor(renderer, 64, 200, 64, 255); // Verde se completado
                } else {
                    SDL_SetRenderDrawColor(renderer, 64, 128, 200, 255); // Azul se disponível
                }
                SDL_RenderFillRect(renderer, &level);
            } else {
                SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
                SDL_RenderFillRect(renderer, &level);
            }

            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderDrawRect(renderer, &level);

            char level_num[8];
            snprintf(level_num, sizeof(level_num), "%d", level_id + 1);
            
            TextColor num_color = TEXT_COLOR_WHITE;
            if (!game->levels[level_id].unlocked) {
                num_color = TEXT_COLOR_GRAY;
            }
            text_render_centered(level_num, level.x, level.y + 10, level.w, TEXT_STYLE_BOLD, num_color);

            const char* status = "";
            TextColor status_color = TEXT_COLOR_WHITE;
            if (!game->levels[level_id].unlocked) {
                status = "BLOQUEADO";
                status_color = TEXT_COLOR_RED;
            } else if (game->levels[level_id].completed) {
                status = "COMPLETO";
                status_color = TEXT_COLOR_GREEN;
            } else {
                status = "DISPONIVEL";
                status_color = TEXT_COLOR_YELLOW;
            }
            
            text_render_centered(status, level.x, level.y + 35, level.w, TEXT_STYLE_SMALL, status_color);

            text_render_centered(game->levels[level_id].theme, level.x, level.y + 55, level.w, TEXT_STYLE_SMALL, TEXT_COLOR_WHITE);

            char difficulty_str[16];
            snprintf(difficulty_str, sizeof(difficulty_str), "IA: %d", game->levels[level_id].ai_difficulty);
            text_render_centered(difficulty_str, level.x, level.y + 75, level.w, TEXT_STYLE_SMALL, TEXT_COLOR_GRAY);

            if (game->levels[level_id].has_powerups) {
                text_render_string("P", level.x + level.w - 30, level.y + 5, TEXT_STYLE_SMALL, TEXT_COLOR_YELLOW, TEXT_ALIGN_LEFT);
            }
            if (game->levels[level_id].has_obstacles) {
                text_render_string("O", level.x + level.w - 15, level.y + 5, TEXT_STYLE_SMALL, TEXT_COLOR_RED, TEXT_ALIGN_LEFT);
            }
        }
    }

    TextColor instruction_color = TEXT_COLOR_GRAY;
    text_render_centered("SETAS PARA NAVEGAR - ENTER PARA JOGAR - ESC PARA VOLTAR", 
                        SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT - 100, 600, TEXT_STYLE_SMALL, instruction_color);

    text_render_string("P = POWERUPS", 50, SCREEN_HEIGHT - 60, TEXT_STYLE_SMALL, TEXT_COLOR_YELLOW, TEXT_ALIGN_LEFT);
    text_render_string("O = OBSTACULOS", 50, SCREEN_HEIGHT - 40, TEXT_STYLE_SMALL, TEXT_COLOR_RED, TEXT_ALIGN_LEFT);

    cheat_system_render(&game->cheat_system, renderer);
}

void render_pause_state_sdl(Game* game, SDL_Renderer* renderer) {
    render_gameplay_state_sdl(game, renderer);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);

    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    SDL_Rect pause_menu = {
        SCREEN_WIDTH / 2 - 150,
        SCREEN_HEIGHT / 2 - 100,
        300, 200
    };
    SDL_RenderFillRect(renderer, &pause_menu);

    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderDrawRect(renderer, &pause_menu);

    TextColor title_color = TEXT_COLOR_WHITE;
    text_render_centered("JOGO PAUSADO", pause_menu.x, pause_menu.y + 20, pause_menu.w, TEXT_STYLE_BOLD, title_color);

    const char* options[] = {"CONTINUAR", "REINICIAR", "MENU PRINCIPAL"};
    for (int i = 0; i < 3; i++) {
        TextColor option_color = TEXT_COLOR_WHITE;
        text_render_centered(options[i], pause_menu.x, pause_menu.y + 70 + i * 30, pause_menu.w, TEXT_STYLE_NORMAL, option_color);
    }

    TextColor instruction_color = TEXT_COLOR_YELLOW;
    text_render_centered("P PARA CONTINUAR", pause_menu.x, pause_menu.y + 160, pause_menu.w, TEXT_STYLE_SMALL, instruction_color);
    text_render_centered("ESC PARA MENU", pause_menu.x, pause_menu.y + 175, pause_menu.w, TEXT_STYLE_SMALL, instruction_color);
}

void render_victory_state_sdl(Game* game, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 50, 40, 10, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        int golden = 50 + (y * 100) / SCREEN_HEIGHT;
        SDL_SetRenderDrawColor(renderer, golden, golden * 0.8f, golden * 0.2f, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }

    TextColor victory_color = TEXT_COLOR_YELLOW;
    text_render_centered("VITORIA!", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 80, 300, TEXT_STYLE_TITLE, victory_color);

    char level_text[64];
    snprintf(level_text, sizeof(level_text), "NIVEL %d COMPLETO!", game->current_level + 1);
    TextColor level_color = TEXT_COLOR_WHITE;
    text_render_centered(level_text, SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 30, 300, TEXT_STYLE_BOLD, level_color);

    char score_text[64];
    snprintf(score_text, sizeof(score_text), "PLACAR FINAL: %d X %d", 
             game->player_paddle.score, game->ai_paddle.score);
    TextColor score_color = TEXT_COLOR_WHITE;
    text_render_centered(score_text, SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 10, 300, TEXT_STYLE_NORMAL, score_color);

    if (game->current_level < MAX_LEVELS - 1 && game->levels[game->current_level + 1].unlocked) {
        TextColor unlock_color = TEXT_COLOR_GREEN;
        text_render_centered("NOVO NIVEL DESBLOQUEADO!", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 40, 300, TEXT_STYLE_NORMAL, unlock_color);
    }

    TextColor instruction_color = TEXT_COLOR_GRAY;
    text_render_centered("ENTER PARA CONTINUAR", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 80, 300, TEXT_STYLE_SMALL, instruction_color);

    static float blink_timer = 0;
    blink_timer += 0.1f;
    if (sinf(blink_timer * 2.0f) > 0) {
        TextColor blink_color = TEXT_COLOR_YELLOW;
        text_render_centered("PARABENS!", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 120, 200, TEXT_STYLE_BOLD, blink_color);
    }
}

void render_defeat_state_sdl(Game* game, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 50, 10, 10, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        int red = 50 + (y * 80) / SCREEN_HEIGHT;
        SDL_SetRenderDrawColor(renderer, red, red * 0.2f, red * 0.2f, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }

    TextColor gameover_color = TEXT_COLOR_RED;
    text_render_centered("GAME OVER", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 80, 300, TEXT_STYLE_TITLE, gameover_color);

    char level_text[64];
    snprintf(level_text, sizeof(level_text), "NIVEL %d FALHOU", game->current_level + 1);
    TextColor level_color = TEXT_COLOR_WHITE;
    text_render_centered(level_text, SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 30, 300, TEXT_STYLE_BOLD, level_color);

    char score_text[64];
    snprintf(score_text, sizeof(score_text), "PLACAR FINAL: %d X %d", 
             game->player_paddle.score, game->ai_paddle.score);
    TextColor score_color = TEXT_COLOR_WHITE;
    text_render_centered(score_text, SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 10, 300, TEXT_STYLE_NORMAL, score_color);

    TextColor motivation_color = TEXT_COLOR_YELLOW;
    text_render_centered("TENTE NOVAMENTE!", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 40, 300, TEXT_STYLE_NORMAL, motivation_color);

    const char* tips[] = {
        "DICA: TENTE REDUZIR A DIFICULDADE",
        "DICA: PRATIQUE MAIS OS MOVIMENTOS",
        "DICA: VOCE ESTA QUASE LA!"
    };
    TextColor tip_color = TEXT_COLOR_GRAY;
    text_render_centered(tips[game->ai_difficulty], SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + 70, 400, TEXT_STYLE_SMALL, tip_color);

    TextColor instruction_color = TEXT_COLOR_GRAY;
    text_render_centered("ENTER PARA CONTINUAR", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 100, 300, TEXT_STYLE_SMALL, instruction_color);
}

void render_gameplay_state_sdl(Game* game, SDL_Renderer* renderer) {
    const LevelData* current_level_data = &LEVELS_DATA[game->current_level];
    
    int floor_r = (int)(current_level_data->floor_color_r * 255);
    int floor_g = (int)(current_level_data->floor_color_g * 255);
    int floor_b = (int)(current_level_data->floor_color_b * 255);
    SDL_SetRenderDrawColor(renderer, floor_r, floor_g, floor_b, 255);
    SDL_RenderClear(renderer);

    int accent_r = (int)(current_level_data->accent_color_r * 255);
    int accent_g = (int)(current_level_data->accent_color_g * 255);
    int accent_b = (int)(current_level_data->accent_color_b * 255);
    SDL_SetRenderDrawColor(renderer, accent_r, accent_g, accent_b, 255);
    for (int y = 0; y < SCREEN_HEIGHT; y += 20) {
        SDL_Rect segment = {SCREEN_WIDTH / 2 - 2, y, 4, 10};
        SDL_RenderFillRect(renderer, &segment);
    }

    Enemy* enemy = &game->enemy;
    if (enemy && enemy->visual_effect.active) {
        render_enemy_power_effects(renderer, enemy);
    }

    SDL_Rect ball = {
        (int)game->ball.x - BALL_SIZE/2, 
        (int)game->ball.y - BALL_SIZE/2, 
        BALL_SIZE, 
        BALL_SIZE
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball);
    
    if (game->waiting_for_dialog) {
        dialog_render(&game->level_dialog, renderer);
    }

    if (game->enemy.fake_balls_active) {
        SDL_SetRenderDrawColor(renderer, 255, 100, 100, 150);
        for (int i = 0; i < 2; i++) {
            SDL_Rect fake_ball = {
                (int)game->enemy.fake_balls[i].x - BALL_SIZE/2,
                (int)game->enemy.fake_balls[i].y - BALL_SIZE/2,
                BALL_SIZE, BALL_SIZE
            };
            SDL_RenderFillRect(renderer, &fake_ball);
        }
    }

    SDL_Rect player_paddle = {
        (int)game->player_paddle.x,
        (int)game->player_paddle.y,
        (int)game->player_paddle.width,
        (int)game->player_paddle.height
    };
    
    if (game->enemy.player_stunned) {
        static float stun_timer = 0;
        stun_timer += 0.1f;
        if ((int)(stun_timer * 10) % 2 == 0) {
            SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }
    SDL_RenderFillRect(renderer, &player_paddle);

    SDL_Rect ai_paddle = {
        (int)game->ai_paddle.x,
        (int)game->ai_paddle.y,
        (int)game->ai_paddle.width,
        (int)game->ai_paddle.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ai_paddle);

    for (int i = 0; i < 5; i++) {
        if (game->powerups[i].active) {
            SDL_Rect powerup = {
                (int)game->powerups[i].x - 15,
                (int)game->powerups[i].y - 15,
                30, 30
            };
            
            static float pulse_timer = 0;
            pulse_timer += 0.1f;
            float pulse = 0.7f + 0.3f * sinf(pulse_timer * 3.0f);
            
            switch (game->powerups[i].type) {
                case 0: // Speed
                    SDL_SetRenderDrawColor(renderer, (int)(255 * pulse), (int)(255 * pulse), 0, 255);
                    break;
                case 1: // Size
                    SDL_SetRenderDrawColor(renderer, 0, (int)(255 * pulse), (int)(255 * pulse), 255);
                    break;
                case 2: // Multi-ball
                    SDL_SetRenderDrawColor(renderer, (int)(255 * pulse), 0, (int)(255 * pulse), 255);
                    break;
            }
            SDL_RenderFillRect(renderer, &powerup);
            
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &powerup);

            const char* powerup_names[] = {"SPEED", "SIZE", "MULTI"};
            TextColor powerup_color = TEXT_COLOR_WHITE;
            text_render_centered(powerup_names[game->powerups[i].type], 
                               powerup.x, powerup.y + 35, powerup.w, TEXT_STYLE_SMALL, powerup_color);
        }
    }
    
    CheatSystem* cheat = &game->cheat_system;
    
    if (cheat->rainbow_mode) {
        static float rainbow_timer = 0;
        rainbow_timer += 0.1f;
        
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        for (int i = 0; i < 10; i++) {
            int r = (int)(128 + 127 * sinf(rainbow_timer + i));
            int g = (int)(128 + 127 * sinf(rainbow_timer + i + 2.0f));
            int b = (int)(128 + 127 * sinf(rainbow_timer + i + 4.0f));
            
            SDL_SetRenderDrawColor(renderer, r, g, b, 50);
            SDL_Rect rainbow_rect = {0, i * (SCREEN_HEIGHT / 10), SCREEN_WIDTH, SCREEN_HEIGHT / 10};
            SDL_RenderFillRect(renderer, &rainbow_rect);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    char score_text[32];
    snprintf(score_text, sizeof(score_text), "JOGADOR: %d", game->player_paddle.score);
    TextColor player_score_color = TEXT_COLOR_WHITE;
    text_render_string(score_text, 50, 20, TEXT_STYLE_NORMAL, player_score_color, TEXT_ALIGN_LEFT);

    snprintf(score_text, sizeof(score_text), "IA: %d", game->ai_paddle.score);
    TextColor ai_score_color = TEXT_COLOR_WHITE;
    text_render_string(score_text, SCREEN_WIDTH - 150, 20, TEXT_STYLE_NORMAL, ai_score_color, TEXT_ALIGN_LEFT);

    char level_text[64];
    snprintf(level_text, sizeof(level_text), "NIVEL %d - %s", 
             game->current_level + 1, game->levels[game->current_level].name);
    TextColor level_color = TEXT_COLOR_YELLOW;
    text_render_centered(level_text, SCREEN_WIDTH / 2 - 150, 5, 300, TEXT_STYLE_SMALL, level_color);

    if (game->enemy.power_active) {
        const char* power_names[] = {
            "NENHUM", "ERRO BOBO", "MURALHA DE ACO", "BOLADA ATORDOANTE",
            "TROCA PERCURSO", "NEBLINA DENSA", "CHOQUE PARALISANTE",
            "BOLA CAOTICA", "CLONE SOMBRAS", "DOMINIO ABSOLUTO"
        };
        
        char power_text[64];
        snprintf(power_text, sizeof(power_text), "PODER: %s", power_names[game->enemy.power_type]);
        TextColor power_color = TEXT_COLOR_RED;
        text_render_centered(power_text, SCREEN_WIDTH / 2 - 100, 45, 200, TEXT_STYLE_SMALL, power_color);
    }

    if (game->enemy.player_stunned) {
        TextColor stun_color = TEXT_COLOR_RED;
        text_render_centered("ATORDOADO!", SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT / 2 - 100, 150, TEXT_STYLE_BOLD, stun_color);
    }

    const char* diff_names[] = {"FACIL", "MEDIO", "DIFICIL"};
    char diff_text[32];
    snprintf(diff_text, sizeof(diff_text), "IA: %s", diff_names[game->ai_difficulty]);
    
    TextColor diff_color;
    switch (game->ai_difficulty) {
        case 0: diff_color = TEXT_COLOR_GREEN; break;   // Verde
        case 1: diff_color = TEXT_COLOR_YELLOW; break;  // Amarelo
        case 2: diff_color = TEXT_COLOR_RED; break;     // Vermelho
        default: diff_color = TEXT_COLOR_WHITE; break;
    }
    text_render_centered(diff_text, SCREEN_WIDTH / 2 - 50, 25, 100, TEXT_STYLE_SMALL, diff_color);

    TextColor control_color = TEXT_COLOR_GRAY;
    text_render_string("W/S OU SETAS", 50, SCREEN_HEIGHT - 40, TEXT_STYLE_SMALL, control_color, TEXT_ALIGN_LEFT);
    text_render_string("P = PAUSA", 50, SCREEN_HEIGHT - 25, TEXT_STYLE_SMALL, control_color, TEXT_ALIGN_LEFT);
    text_render_string("ESC = MENU", 50, SCREEN_HEIGHT - 10, TEXT_STYLE_SMALL, control_color, TEXT_ALIGN_LEFT);

    if (game->sound_enabled) {
        text_render_string("SOM: ON", SCREEN_WIDTH - 100, SCREEN_HEIGHT - 25, TEXT_STYLE_SMALL, TEXT_COLOR_GREEN, TEXT_ALIGN_LEFT);
    } else {
        text_render_string("SOM: OFF", SCREEN_WIDTH - 100, SCREEN_HEIGHT - 25, TEXT_STYLE_SMALL, TEXT_COLOR_RED, TEXT_ALIGN_LEFT);
    }
    
    int cheat_indicator_y = SCREEN_HEIGHT - 120;
    if (cheat->godmode_paddle) {
        TextColor godmode_color = TEXT_COLOR_GREEN;
        text_render_string("GODMODE", 10, cheat_indicator_y, TEXT_STYLE_SMALL, godmode_color, TEXT_ALIGN_LEFT);
        cheat_indicator_y -= 15;
    }
    if (cheat->ai_frozen) {
        TextColor freeze_color = TEXT_COLOR_CYAN;
        text_render_string("AI FROZEN", 10, cheat_indicator_y, TEXT_STYLE_SMALL, freeze_color, TEXT_ALIGN_LEFT);
        cheat_indicator_y -= 15;
    }
    if (cheat->super_ball) {
        TextColor superball_color = TEXT_COLOR_RED;
        text_render_string("SUPER BALL", 10, cheat_indicator_y, TEXT_STYLE_SMALL, superball_color, TEXT_ALIGN_LEFT);
        cheat_indicator_y -= 15;
    }
    if (cheat->noclip_mode) {
        TextColor noclip_color = TEXT_COLOR_YELLOW;
        text_render_string("NOCLIP", 10, cheat_indicator_y, TEXT_STYLE_SMALL, noclip_color, TEXT_ALIGN_LEFT);
        cheat_indicator_y -= 15;
    }
    if (cheat->slow_motion) {
        TextColor slowmo_color = TEXT_COLOR_BLUE;
        text_render_string("SLOW MOTION", 10, cheat_indicator_y, TEXT_STYLE_SMALL, slowmo_color, TEXT_ALIGN_LEFT);
        cheat_indicator_y -= 15;
    }
    if (cheat->ball_control) {
        TextColor ballcontrol_color = TEXT_COLOR_PURPLE;
        text_render_string("BALL CONTROL", 10, cheat_indicator_y, TEXT_STYLE_SMALL, ballcontrol_color, TEXT_ALIGN_LEFT);
        text_render_string("SETAS: INFLUENCIAR BOLA", 10, cheat_indicator_y - 15, TEXT_STYLE_SMALL, TEXT_COLOR_GRAY, TEXT_ALIGN_LEFT);
    }

    cheat_system_render(&game->cheat_system, renderer);
}

void render_enemy_power_effects(SDL_Renderer* renderer, Enemy* enemy) {
    if (!enemy) return;
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    if (enemy->pre_effect.active) {
        float progress = (enemy->pre_effect.duration - enemy->pre_effect.timer) / enemy->pre_effect.duration;
        float oscillation = sinf(progress * 20.0f) * 0.4f + 0.6f; // Oscilação mais visível
        
        switch (enemy->pre_effect.effect_type) {
            case 1: // POWER_ERRO_BOBO - Aura roxa na bola
                {
                    int r = (int)(153 * oscillation);
                    int g = (int)(51 * oscillation);
                    int b = (int)(204 * oscillation);
                    int a = (int)(200 * oscillation);
                    
                    SDL_SetRenderDrawColor(renderer, r, g, b, a);
                    
                    float base_radius = 20.0f + (progress * 15.0f);
                    for (int ring = 0; ring < 4; ring++) {
                        float ring_radius = base_radius + (ring * 8);
                        
                        for (int angle = 0; angle < 360; angle += 15) {
                            float rad = angle * M_PI / 180.0f;
                            int x = (int)(enemy->pre_effect.pos_x + cosf(rad) * ring_radius);
                            int y = (int)(enemy->pre_effect.pos_y + sinf(rad) * ring_radius);
                            
                            SDL_Rect dot = {x - 2, y - 2, 4, 4};
                            SDL_RenderFillRect(renderer, &dot);
                        }
                    }
                    
                    SDL_SetRenderDrawColor(renderer, 255, 100, 255, (int)(255 * oscillation));
                    SDL_Rect core = {
                        (int)enemy->pre_effect.pos_x - 8,
                        (int)enemy->pre_effect.pos_y - 8,
                        16, 16
                    };
                    SDL_RenderFillRect(renderer, &core);
                }
                break;
                
            case 2: // POWER_BOLADA_ATORDOANTE - Aura vermelha na bola
                {
                    int r = (int)(255 * oscillation);
                    int g = (int)(50 * oscillation);
                    int b = (int)(50 * oscillation);
                    int a = (int)(220 * oscillation);
                    
                    SDL_SetRenderDrawColor(renderer, r, g, b, a);
                    
                    float base_radius = 25.0f + (progress * 20.0f);
                    for (int ring = 0; ring < 5; ring++) {
                        float ring_radius = base_radius + (ring * 6);
                        
                        for (int angle = 0; angle < 360; angle += 12) {
                            float rad = angle * M_PI / 180.0f;
                            int x = (int)(enemy->pre_effect.pos_x + cosf(rad) * ring_radius);
                            int y = (int)(enemy->pre_effect.pos_y + sinf(rad) * ring_radius);
                            
                            SDL_Rect dot = {x - 3, y - 3, 6, 6};
                            SDL_RenderFillRect(renderer, &dot);
                        }
                    }
                    
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, (int)(255 * oscillation));
                    SDL_Rect core = {
                        (int)enemy->pre_effect.pos_x - 10,
                        (int)enemy->pre_effect.pos_y - 10,
                        20, 20
                    };
                    SDL_RenderFillRect(renderer, &core);
                }
                break;
                
            case 3: // POWER_BOLA_CAOTICA - Aura verde na bola
                {
                    int r = (int)(50 * oscillation);
                    int g = (int)(255 * oscillation);
                    int b = (int)(50 * oscillation);
                    int a = (int)(200 * oscillation);
                    
                    SDL_SetRenderDrawColor(renderer, r, g, b, a);
                    
                    float base_radius = 22.0f + (progress * 18.0f);
                    for (int ring = 0; ring < 4; ring++) {
                        float ring_radius = base_radius + (ring * 7);
                        
                        for (int angle = 0; angle < 360; angle += 10) {
                            float rad = angle * M_PI / 180.0f;
                            int x = (int)(enemy->pre_effect.pos_x + cosf(rad) * ring_radius);
                            int y = (int)(enemy->pre_effect.pos_y + sinf(rad) * ring_radius);
                            
                            SDL_Rect dot = {x - 2, y - 2, 4, 4};
                            SDL_RenderFillRect(renderer, &dot);
                        }
                    }
                    
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, (int)(255 * oscillation));
                    SDL_Rect core = {
                        (int)enemy->pre_effect.pos_x - 9,
                        (int)enemy->pre_effect.pos_y - 9,
                        18, 18
                    };
                    SDL_RenderFillRect(renderer, &core);
                }
                break;
                
            case 4: // POWER_MURALHA_ACO - Sombra onde a muralha aparecerá
                {
                    int shadow_alpha = (int)(150 * oscillation);
                    SDL_SetRenderDrawColor(renderer, 80, 80, 80, shadow_alpha);
                    
                    float shadow_width = 30.0f * progress;
                    float shadow_height = 320.0f * progress;
                    
                    SDL_Rect shadow = {
                        (int)(350.0f - shadow_width/2),
                        (int)(300.0f - shadow_height/2),
                        (int)shadow_width,
                        (int)shadow_height
                    };
                    SDL_RenderFillRect(renderer, &shadow);
                    
                    SDL_SetRenderDrawColor(renderer, 150, 150, 200, (int)(200 * oscillation));
                    SDL_RenderDrawRect(renderer, &shadow);
                }
                break;
                
            case 6: // POWER_TROCA_PERCURSO - Efeito de glitch CORRIGIDO
                {
                    static float glitch_timer = 0.0f;
                    glitch_timer += 0.1f;
                    
                    for (int glitch = 0; glitch < 8; glitch++) {
                        int offset_x = (rand() % 40) - 20; // -20 a +20
                        int offset_y = (rand() % 40) - 20; // -20 a +20
                        
                        int r = 200 + (rand() % 55); // 200-255
                        int g = rand() % 100;         // 0-99
                        int b = 200 + (rand() % 55); // 200-255
                        int a = (int)(180 + (oscillation * 75)); // 180-255
                        
                        SDL_SetRenderDrawColor(renderer, r, g, b, a);
                        
                        SDL_Rect glitch_rect = {
                            (int)(enemy->pre_effect.pos_x + offset_x - 12),
                            (int)(enemy->pre_effect.pos_y + offset_y - 12),
                            24, 24
                        };
                        SDL_RenderFillRect(renderer, &glitch_rect);
                        
                        if (glitch % 2 == 0) {
                            SDL_SetRenderDrawColor(renderer, 255, 0, 255, (int)(150 * oscillation));
                            SDL_RenderDrawLine(renderer,
                                (int)(enemy->pre_effect.pos_x - 30 + offset_x),
                                (int)(enemy->pre_effect.pos_y + offset_y),
                                (int)(enemy->pre_effect.pos_x + 30 + offset_x),
                                (int)(enemy->pre_effect.pos_y + offset_y));
                        }
                        
                        if (glitch % 3 == 0) {
                            SDL_SetRenderDrawColor(renderer, 0, 255, 255, (int)(150 * oscillation));
                            SDL_RenderDrawLine(renderer,
                                (int)(enemy->pre_effect.pos_x + offset_x),
                                (int)(enemy->pre_effect.pos_y - 30 + offset_y),
                                (int)(enemy->pre_effect.pos_x + offset_x),
                                (int)(enemy->pre_effect.pos_y + 30 + offset_y));
                        }
                    }
                    
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, (int)(200 * oscillation));
                    SDL_Rect core = {
                        (int)enemy->pre_effect.pos_x - 6,
                        (int)enemy->pre_effect.pos_y - 6,
                        12, 12
                    };
                    SDL_RenderFillRect(renderer, &core);
                }
                break;
        }
    }
    
    if (enemy->power_active && enemy->power_type == POWER_CHOQUE_PARALISANTE) {
        static float electric_timer = 0.0f;
        electric_timer += 0.1f;
        bool is_white_phase = ((int)(electric_timer * 10) % 2) == 0;
        
        if (is_white_phase) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255);
        }
        
        float start_x = enemy->paddle.x;
        float start_y = enemy->paddle.y + enemy->paddle.height / 2;
        float end_x = enemy->visual_effect.pos_x; // Posição do jogador salva no efeito
        float end_y = enemy->visual_effect.pos_y;
        
        SDL_RenderDrawLine(renderer, 
            (int)start_x, (int)start_y,
            (int)end_x, (int)end_y);
        
        for (int i = 0; i < 5; i++) {
            int offset_y = (rand() % 8) - 4;
            int offset_x = (rand() % 6) - 3;
            SDL_RenderDrawLine(renderer,
                (int)start_x + offset_x, (int)start_y + offset_y,
                (int)end_x + offset_x, (int)end_y + offset_y);
        }
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 200);
        for (int i = 0; i < 3; i++) {
            SDL_RenderDrawLine(renderer,
                (int)start_x, (int)start_y + i - 1,
                (int)end_x, (int)end_y + i - 1);
        }
    }
    
    if (enemy->visual_effect.active) {
        int r = (int)(enemy->visual_effect.color_r * 255);
        int g = (int)(enemy->visual_effect.color_g * 255);
        int b = (int)(enemy->visual_effect.color_b * 255);
        int a = (int)(enemy->visual_effect.intensity * 128);
        
        switch (enemy->visual_effect.effect_type) {
            case 2: // Muralha de Aço - barreira
                if (enemy->barrier.active) {
                    SDL_SetRenderDrawColor(renderer, r, g, b, 200);
                    SDL_Rect barrier = {
                        (int)enemy->barrier.x, (int)enemy->barrier.y,
                        (int)enemy->barrier.width, (int)enemy->barrier.height
                    };
                    SDL_RenderFillRect(renderer, &barrier);
                    
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 150);
                    SDL_RenderDrawRect(renderer, &barrier);
                }
                break;
                
            case 5: // Neblina Densa - overlay
                {
                    SDL_SetRenderDrawColor(renderer, r, g, b, (int)(enemy->fog_intensity * 120));
                    SDL_Rect fog_overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
                    SDL_RenderFillRect(renderer, &fog_overlay);
                }
                break;
                
            case 7: // Bola Caótica - múltiplas bolas
                if (enemy->fake_balls_active) {
                    SDL_SetRenderDrawColor(renderer, r, g, b, 150);
                    for (int i = 0; i < 2; i++) {
                        SDL_Rect fake_ball = {
                            (int)enemy->fake_balls[i].x - 7,
                            (int)enemy->fake_balls[i].y - 7,
                            14, 14
                        };
                        SDL_RenderFillRect(renderer, &fake_ball);
                    }
                }
                break;
                
            case 8: // Clone das Sombras
                if (enemy->clone.active) {
                    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 180);
                    SDL_Rect clone_paddle = {
                        (int)enemy->clone.x, (int)enemy->clone.y,
                        (int)enemy->clone.width, (int)enemy->clone.height 
                    };
                    SDL_RenderFillRect(renderer, &clone_paddle);
                    
                    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 200);
                    SDL_RenderDrawRect(renderer, &clone_paddle);
                }
                break;
                
            case 9: // Domínio Absoluto - aura multicolorida
                {
                    static float rainbow_timer = 0;
                    rainbow_timer += 0.1f;
                    
                    int rainbow_r = (int)(128 + 127 * sinf(rainbow_timer));
                    int rainbow_g = (int)(128 + 127 * sinf(rainbow_timer + 2.0f));
                    int rainbow_b = (int)(128 + 127 * sinf(rainbow_timer + 4.0f));
                    
                    SDL_SetRenderDrawColor(renderer, rainbow_r, rainbow_g, rainbow_b, 100);
                    SDL_Rect aura = {
                        (int)enemy->paddle.x - 20, (int)enemy->paddle.y - 20,
                        60, 140
                    };
                    SDL_RenderFillRect(renderer, &aura);
                }
                break;
        }
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void render_splash_state(Game* game, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 64, 128, 255, 255);
    SDL_Rect logo = {
        SCREEN_WIDTH / 2 - 150,
        SCREEN_HEIGHT / 2 - 75,
        300, 150
    };
    SDL_RenderFillRect(renderer, &logo);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect title = {
        SCREEN_WIDTH / 2 - 100,
        SCREEN_HEIGHT / 2 - 25,
        200, 50
    };
    SDL_RenderFillRect(renderer, &title);
}

void render_menu_state(Game* game, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);

    const char* options[] = {"Jogar", "Tutorial", "Rankings", "Sair"};
    for (int i = 0; i < 4; i++) {
        SDL_Rect item = {
            SCREEN_WIDTH / 2 - 100,
            SCREEN_HEIGHT / 2 - 80 + i * 40,
            200, 30
        };

        if (i == game->selected_menu_item) {
            SDL_SetRenderDrawColor(renderer, 64, 128, 255, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
        }
        SDL_RenderFillRect(renderer, &item);
    }
}

void render_level_select_state(Game* game, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
            int level_id = y * 3 + x;
            if (level_id >= MAX_LEVELS) break;

            SDL_Rect level = {
                200 + x * 100,
                150 + y * 100,
                80, 80
            };

            if (game->levels[level_id].unlocked) {
                if (level_id == game->current_level) {
                    SDL_SetRenderDrawColor(renderer, 255, 128, 64, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 64, 128, 64, 255);
                }
            } else {
                SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
            }

            SDL_RenderFillRect(renderer, &level);
        }
    }
}

void render_pause_state(Game* game, SDL_Renderer* renderer) {
    render_gameplay_state(game, renderer);

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

void render_victory_state(Game* game, SDL_Renderer* renderer) {
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

void render_defeat_state(Game* game, SDL_Renderer* renderer) {
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


void state_splash_update(Game* game, float delta_time) {
    static float splash_timer = 0;
    splash_timer += delta_time;

    if (splash_timer > 3.0f) {
        game->current_state = STATE_MAIN_MENU;
        printf("Auto transição: Splash -> Main Menu\n");
    }
}

void state_main_menu_update(Game* game, float delta_time) {
}

void state_level_select_update(Game* game, float delta_time) {
}

void pvp_config_init(PvPConfig* config) {
    config->paddle_speed = 300.0f;
    config->ball_speed = 300.0f;
    config->powers_enabled = false;
    config->points_to_win = 5;
    config->selected_config_item = 0;
    
    config->player1_power = POWER_NONE;
    config->player2_power = POWER_NONE;
    config->player1_power_cooldown = 0.0f;
    config->player2_power_cooldown = 0.0f;
    config->player1_power_active = false;
    config->player2_power_active = false;
    config->player1_power_duration = 0.0f;
    config->player2_power_duration = 0.0f;
    
    config->player1_score = 0;
    config->player2_score = 0;
    config->winner = 0;
}

void render_pvp_config_state_sdl(Game* game, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 25, 25, 45, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        int color = 25 + (y * 25) / SCREEN_HEIGHT;
        SDL_SetRenderDrawColor(renderer, color, color, color + 20, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }

    TextColor title_color = TEXT_COLOR_WHITE;
    text_render_centered("CONFIGURACAO PVP", SCREEN_WIDTH / 2 - 150, 60, 300, TEXT_STYLE_TITLE, title_color);

    char config_texts[6][80];
    snprintf(config_texts[0], 80, "VELOCIDADE RAQUETES: %.0f", game->pvp_config.paddle_speed);
    snprintf(config_texts[1], 80, "VELOCIDADE BOLA: %.0f", game->pvp_config.ball_speed);
    snprintf(config_texts[2], 80, "PODERES: %s", game->pvp_config.powers_enabled ? "ATIVADO" : "DESATIVADO");
    snprintf(config_texts[3], 80, "PONTOS PARA VENCER: %d", game->pvp_config.points_to_win);
    strcpy(config_texts[4], "JOGAR");
    strcpy(config_texts[5], "VOLTAR");

    for (int i = 0; i < 6; i++) {
        SDL_Rect item = {
            SCREEN_WIDTH / 2 - 200,
            140 + i * 70,
            400, 60
        };

        if (i == game->pvp_config.selected_config_item) {
            SDL_SetRenderDrawColor(renderer, 64, 128, 255, 255);
            SDL_RenderFillRect(renderer, &item);
            
            SDL_SetRenderDrawColor(renderer, 128, 164, 255, 255);
            for (int border = 0; border < 3; border++) {
                SDL_Rect border_rect = {
                    item.x - border, item.y - border,
                    item.w + 2 * border, item.h + 2 * border
                };
                SDL_RenderDrawRect(renderer, &border_rect);
            }

            TextColor selected_color = TEXT_COLOR_WHITE;
            text_render_centered(config_texts[i], item.x, item.y + 20, item.w, TEXT_STYLE_BOLD, selected_color);

            if (i < 4) {
                TextColor hint_color = TEXT_COLOR_YELLOW;
                text_render_centered("ESQUERDA/DIREITA PARA ALTERAR", item.x, item.y + 40, item.w, TEXT_STYLE_SMALL, hint_color);
            }
        } else {
            if (i == 4) { // JOGAR
                SDL_SetRenderDrawColor(renderer, 40, 80, 40, 255);
            } else if (i == 5) { // VOLTAR
                SDL_SetRenderDrawColor(renderer, 80, 40, 40, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
            }
            SDL_RenderFillRect(renderer, &item);

            SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
            SDL_RenderDrawRect(renderer, &item);

            TextColor normal_color = TEXT_COLOR_WHITE;
            text_render_centered(config_texts[i], item.x, item.y + 20, item.w, TEXT_STYLE_NORMAL, normal_color);
        }
    }

    TextColor instruction_color = TEXT_COLOR_GRAY;
    text_render_centered("W/S NAVEGAR - ENTER SELECIONAR - ESC VOLTAR", 
                        SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT - 60, 500, TEXT_STYLE_SMALL, instruction_color);

    if (game->pvp_config.powers_enabled) {
        const char* power_names[] = {
            "NENHUM", "ERRO BOBO", "MURALHA DE ACO", "BOLADA ATORDOANTE",
            "TROCA PERCURSO", "NEBLINA DENSA", "CHOQUE PARALISANTE",
            "BOLA CAOTICA", "CLONE SOMBRAS", "DOMINIO ABSOLUTO"
        };
        
        char power_text[128];
        snprintf(power_text, 128, "PLAYER 1: %s | PLAYER 2: %s", 
                power_names[game->pvp_config.player1_power],
                power_names[game->pvp_config.player2_power]);
        
        TextColor power_color = TEXT_COLOR_YELLOW;
        text_render_centered(power_text, SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT - 100, 400, TEXT_STYLE_SMALL, power_color);
    }
}

void render_pvp_gameplay_state_sdl(Game* game, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 20, 60, 20, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int y = 0; y < SCREEN_HEIGHT; y += 20) {
        SDL_Rect segment = {SCREEN_WIDTH / 2 - 2, y, 4, 10};
        SDL_RenderFillRect(renderer, &segment);
    }

    SDL_Rect ball = {
        (int)game->ball.x - BALL_SIZE/2, 
        (int)game->ball.y - BALL_SIZE/2, 
        BALL_SIZE, 
        BALL_SIZE
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball);

    SDL_Rect player1_paddle = {
        (int)game->player_paddle.x,
        (int)game->player_paddle.y,
        (int)game->player_paddle.width,
        (int)game->player_paddle.height
    };
    SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255); // Azul
    SDL_RenderFillRect(renderer, &player1_paddle);

    SDL_Rect player2_paddle = {
        (int)game->ai_paddle.x,
        (int)game->ai_paddle.y,
        (int)game->ai_paddle.width,
        (int)game->ai_paddle.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 150, 100, 255); // Laranja
    SDL_RenderFillRect(renderer, &player2_paddle);

    char score_text[32];
    snprintf(score_text, sizeof(score_text), "PLAYER 1: %d", game->pvp_config.player1_score);
    TextColor player1_color = TEXT_COLOR_CYAN;
    text_render_string(score_text, 50, 20, TEXT_STYLE_NORMAL, player1_color, TEXT_ALIGN_LEFT);

    snprintf(score_text, sizeof(score_text), "PLAYER 2: %d", game->pvp_config.player2_score);
    TextColor player2_color = TEXT_COLOR_ORANGE;
    text_render_string(score_text, SCREEN_WIDTH - 200, 20, TEXT_STYLE_NORMAL, player2_color, TEXT_ALIGN_LEFT);

    TextColor title_color = TEXT_COLOR_WHITE;
    text_render_centered("MODO PVP", SCREEN_WIDTH / 2 - 75, 5, 150, TEXT_STYLE_BOLD, title_color);

    TextColor control_color = TEXT_COLOR_GRAY;
    text_render_string("PLAYER 1: W/S", 50, SCREEN_HEIGHT - 60, TEXT_STYLE_SMALL, control_color, TEXT_ALIGN_LEFT);
    text_render_string("PLAYER 2: SETAS", SCREEN_WIDTH - 150, SCREEN_HEIGHT - 60, TEXT_STYLE_SMALL, control_color, TEXT_ALIGN_LEFT);
    text_render_string("ESC = SAIR", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT - 30, TEXT_STYLE_SMALL, control_color, TEXT_ALIGN_LEFT);

    if (game->pvp_config.powers_enabled) {
        if (game->pvp_config.player1_power_active) {
            TextColor p1_power_color = TEXT_COLOR_CYAN;
            text_render_string("PODER P1 ATIVO", 50, 50, TEXT_STYLE_SMALL, p1_power_color, TEXT_ALIGN_LEFT);
        }
        if (game->pvp_config.player2_power_active) {
            TextColor p2_power_color = TEXT_COLOR_ORANGE;
            text_render_string("PODER P2 ATIVO", SCREEN_WIDTH - 150, 50, TEXT_STYLE_SMALL, p2_power_color, TEXT_ALIGN_LEFT);
        }
    }
}

void render_pvp_victory_state_sdl(Game* game, SDL_Renderer* renderer) {
    if (game->pvp_config.winner == 1) {
        SDL_SetRenderDrawColor(renderer, 20, 40, 80, 255); // Azul para Player 1
    } else {
        SDL_SetRenderDrawColor(renderer, 80, 40, 20, 255); // Laranja para Player 2
    }
    SDL_RenderClear(renderer);

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        int color_factor = 50 + (y * 30) / SCREEN_HEIGHT;
        if (game->pvp_config.winner == 1) {
            SDL_SetRenderDrawColor(renderer, color_factor/2, color_factor, color_factor + 20, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, color_factor + 20, color_factor, color_factor/2, 255);
        }
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }

    char victory_text[64];
    snprintf(victory_text, sizeof(victory_text), "PLAYER %d VENCEU!", game->pvp_config.winner);
    
    TextColor victory_color = (game->pvp_config.winner == 1) ? TEXT_COLOR_CYAN : TEXT_COLOR_ORANGE;
    text_render_centered(victory_text, SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 60, 300, TEXT_STYLE_TITLE, victory_color);

    char final_score[64];
    snprintf(final_score, sizeof(final_score), "PLACAR FINAL: %d X %d", 
             game->pvp_config.player1_score, game->pvp_config.player2_score);
    TextColor score_color = TEXT_COLOR_WHITE;
    text_render_centered(final_score, SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, 300, TEXT_STYLE_NORMAL, score_color);

    TextColor instruction_color = TEXT_COLOR_GRAY;
    text_render_centered("ENTER PARA CONTINUAR", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 80, 200, TEXT_STYLE_SMALL, instruction_color);

    static float pulse_timer = 0;
    pulse_timer += 0.1f;
    if (sinf(pulse_timer * 3.0f) > 0) {
        TextColor pulse_color = TEXT_COLOR_YELLOW;
        text_render_centered("PARABENS!", SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT / 2 + 40, 150, TEXT_STYLE_BOLD, pulse_color);
    }
}

void state_pvp_config_update(Game* game, float delta_time) {
}

void state_pvp_gameplay_update(Game* game, float delta_time) {
    float movement_speed = game->pvp_config.paddle_speed * delta_time;
    
    if (game->keys[0] && game->player_paddle.y > 0) { // W
        game->player_paddle.y -= movement_speed;
    }
    if (game->keys[1] && game->player_paddle.y < SCREEN_HEIGHT - game->player_paddle.height) { // S
        game->player_paddle.y += movement_speed;
    }

    if (game->keys[2] && game->ai_paddle.y > 0) { // UP arrow
        game->ai_paddle.y -= movement_speed;
    }
    if (game->keys[3] && game->ai_paddle.y < SCREEN_HEIGHT - game->ai_paddle.height) { // DOWN arrow
        game->ai_paddle.y += movement_speed;
    }

    if (game->ball.active) {
        Ball temp_ball = game->ball;
        temp_ball.vx = (temp_ball.vx > 0) ? game->pvp_config.ball_speed : -game->pvp_config.ball_speed;
        ball_update(&temp_ball, delta_time);
        game->ball = temp_ball;
    }

    check_pvp_collisions(game);

    if (game->pvp_config.player1_score >= game->pvp_config.points_to_win) {
        game->pvp_config.winner = 1;
        game->current_state = STATE_PVP_VICTORY;
        printf("Player 1 venceu!\n");
    } else if (game->pvp_config.player2_score >= game->pvp_config.points_to_win) {
        game->pvp_config.winner = 2;
        game->current_state = STATE_PVP_VICTORY;
        printf("Player 2 venceu!\n");
    }

    if (game->pvp_config.powers_enabled) {
        pvp_update_powers(game, delta_time);
    }
}

void state_pvp_victory_update(Game* game, float delta_time) {
    static float victory_timer = 0;
    victory_timer += delta_time;

    if (victory_timer > 5.0f) {
        victory_timer = 0;
        game->current_state = STATE_PVP_CONFIG;
        
        game->pvp_config.player1_score = 0;
        game->pvp_config.player2_score = 0;
        game->pvp_config.winner = 0;
        
        printf("Retornando à configuração PvP...\n");
    }
}


void check_pvp_collisions(Game* game) {
    Ball* ball = &game->ball;

    if (ball->x < 0) {
        game->pvp_config.player2_score++;
        printf("Player 2 marcou! Placar: %d x %d\n", 
               game->pvp_config.player1_score, game->pvp_config.player2_score);
        
        ball_init(ball, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        SDL_Delay(500);
        return;
    }
    
    if (ball->x > SCREEN_WIDTH) {
        game->pvp_config.player1_score++;
        printf("Player 1 marcou! Placar: %d x %d\n", 
               game->pvp_config.player1_score, game->pvp_config.player2_score);
        
        ball_init(ball, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        SDL_Delay(500);
        return;
    }

    
    if (ball->y <= ball->radius) {
        ball->y = ball->radius;
        ball_reverse_y(ball);
    }
    if (ball->y >= SCREEN_HEIGHT - ball->radius) {
        ball->y = SCREEN_HEIGHT - ball->radius;
        ball_reverse_y(ball);
    }

    if (ball->x - ball->radius <= game->player_paddle.x + game->player_paddle.width &&
        ball->x + ball->radius >= game->player_paddle.x &&
        ball->y >= game->player_paddle.y &&
        ball->y <= game->player_paddle.y + game->player_paddle.height &&
        ball->vx < 0) {

        ball->vx = game->pvp_config.ball_speed;
        ball->x = game->player_paddle.x + game->player_paddle.width + ball->radius;

        float impact_pos = (ball->y - game->player_paddle.y) / game->player_paddle.height;
        float angle_variation = (impact_pos - 0.5f) * 400;
        ball->vy = angle_variation;

        printf("Rebatida Player 1!\n");
    }

    if (ball->x + ball->radius >= game->ai_paddle.x &&
        ball->x - ball->radius <= game->ai_paddle.x + game->ai_paddle.width &&
        ball->y >= game->ai_paddle.y &&
        ball->y <= game->ai_paddle.y + game->ai_paddle.height &&
        ball->vx > 0) {

        ball->vx = -game->pvp_config.ball_speed;
        ball->x = game->ai_paddle.x - ball->radius;

        float impact_pos = (ball->y - game->ai_paddle.y) / game->ai_paddle.height;
        float angle_variation = (impact_pos - 0.5f) * 400;
        ball->vy = angle_variation;

        printf("Rebatida Player 2!\n");
    }
}

void pvp_generate_random_powers(PvPConfig* config) {
    EnemyPowerType available_powers[] = {
        POWER_ERRO_BOBO,
        POWER_MURALHA_ACO,
        POWER_BOLADA_ATORDOANTE,
        POWER_TROCA_PERCURSO,
        POWER_NEBLINA_DENSA,
        POWER_CHOQUE_PARALISANTE,
        POWER_BOLA_CAOTICA,
        POWER_CLONE_SOMBRAS
    };
    
    int num_powers = sizeof(available_powers) / sizeof(EnemyPowerType);
    config->player1_power = available_powers[rand() % num_powers];
    config->player2_power = available_powers[rand() % num_powers];
    
    printf("Poderes gerados - P1: %d, P2: %d\n", config->player1_power, config->player2_power);
}

void pvp_update_powers(Game* game, float delta_time) {
    PvPConfig* config = &game->pvp_config;
    
    if (config->player1_power_cooldown > 0) {
        config->player1_power_cooldown -= delta_time;
    }
    if (config->player2_power_cooldown > 0) {
        config->player2_power_cooldown -= delta_time;
    }
    
    if (config->player1_power_active) {
        config->player1_power_duration -= delta_time;
        if (config->player1_power_duration <= 0) {
            config->player1_power_active = false;
        }
    }
    
    if (config->player2_power_active) {
        config->player2_power_duration -= delta_time;
        if (config->player2_power_duration <= 0) {
            config->player2_power_active = false;
        }
    }
    
    if (config->player1_power_cooldown <= 0 && !config->player1_power_active) {
        config->player1_power_active = true;
        config->player1_power_duration = 3.0f;
        config->player1_power_cooldown = 10.0f;
        printf("Poder Player 1 ativado!\n");
    }
    
    if (config->player2_power_cooldown <= 0 && !config->player2_power_active) {
        config->player2_power_active = true;
        config->player2_power_duration = 3.0f;
        config->player2_power_cooldown = 10.0f;
        printf("Poder Player 2 ativado!\n");
    }
}



void state_gameplay_update(Game* game, float delta_time) {
    CheatSystem* cheat = &game->cheat_system;
    
    if (cheat->all_levels_unlocked) {
        for (int i = 0; i < MAX_LEVELS; i++) {
            game->levels[i].unlocked = true;
        }
    }
    
    if (cheat->instant_win) {
        game->player_paddle.score = 5;
        cheat->instant_win = false; // Reset after use
    }
    
    if (cheat->godmode_paddle) {
        game->player_paddle.height = SCREEN_HEIGHT;
        game->player_paddle.y = 0;
    }
    
    if (cheat->giant_paddle && !cheat->godmode_paddle) {
        game->player_paddle.height = 200;
    }
    
    float effective_delta = delta_time;
    if (cheat->slow_motion) {
        effective_delta *= 0.3f; // 30% da velocidade normal
    }
    
    if (game->waiting_for_dialog) {
        dialog_update(&game->level_dialog, delta_time);
        if (dialog_is_finished(&game->level_dialog)) {
            game->waiting_for_dialog = false;
            printf("Diálogo finalizado automaticamente\n");
        }
    }

    if (game->player_paddle.score >= 5) {
        game->levels[game->current_level].completed = true;
        
        unlock_next_level(game, game->current_level);
        
        game->current_state = STATE_VICTORY;
        printf("VITÓRIA! Jogador ganhou!\n");
        visual_effects_activate_particles(&game->visual_effects, 100);
        return;
    } else if (game->ai_paddle.score >= 5) {
        game->current_state = STATE_GAME_OVER;
        printf("DERROTA! AI ganhou!\n");
        visual_effects_activate_shake(&game->visual_effects, 2.0f);
        return;
    }
    
    if (!cheat->ai_frozen) {
        enemy_update(&game->enemy, &game->ball, &game->player_paddle, effective_delta);
    }

    bool player_stunned = game->enemy.player_stunned;
    
    if (!player_stunned && !game->waiting_for_dialog) {
        float movement_speed = game->player_paddle.speed * game->game_speed;
        
        bool use_ws_only = cheat->ball_control;
        
        if (use_ws_only) {
            if (game->keys[0] && game->player_paddle.y > 0) { // W
                game->player_paddle.y -= movement_speed * delta_time;
            }
            if (game->keys[1] && game->player_paddle.y < SCREEN_HEIGHT - game->player_paddle.height) { // S
                game->player_paddle.y += movement_speed * delta_time;
            }
        } else {
            if (game->keys[0] && game->player_paddle.y > 0) { // W ou UP
                game->player_paddle.y -= movement_speed * delta_time;
            }
            if (game->keys[1] && game->player_paddle.y < SCREEN_HEIGHT - game->player_paddle.height) { // S ou DOWN
                game->player_paddle.y += movement_speed * delta_time;
            }
        }
    } else if (player_stunned) {
        printf("Jogador atordoado!\n");
    }

    if (game->player_paddle.y < 0) {
        game->player_paddle.y = 0;
    }
    if (game->player_paddle.y > SCREEN_HEIGHT - game->player_paddle.height) {
        game->player_paddle.y = SCREEN_HEIGHT - game->player_paddle.height;
    }

    if (cheat->ball_control && !game->waiting_for_dialog) {
        float ball_influence = 300.0f; // Força da influência do jogador na bola
        
        if (game->keys[2]) { // UP arrow - influenciar bola para cima
            game->ball.vy -= ball_influence * delta_time;
            printf("Influenciando bola para CIMA!\n");
        }
        if (game->keys[3]) { // DOWN arrow - influenciar bola para baixo
            game->ball.vy += ball_influence * delta_time;
            printf("Influenciando bola para BAIXO!\n");
        }
        
        if (game->ball.vy > 600.0f) game->ball.vy = 600.0f;
        if (game->ball.vy < -600.0f) game->ball.vy = -600.0f;
    }

    if (game->ball.active && !game->waiting_for_dialog) {
        Ball temp_ball = game->ball;
        
        if (cheat->super_ball) {
            temp_ball.vx *= 3.0f;
            temp_ball.vy *= 3.0f;
        }
        
        temp_ball.vx *= game->game_speed;
        temp_ball.vy *= game->game_speed;
        
        ball_update(&temp_ball, effective_delta);
        
        game->ball.x = temp_ball.x;
        game->ball.y = temp_ball.y;
    }
    
    if (!game->waiting_for_dialog && !cheat->ai_frozen) {
        Paddle temp_ai = game->ai_paddle;
        temp_ai.speed *= game->game_speed;
        paddle_update_ai(&temp_ai, &game->ball, game->ai_difficulty, effective_delta);
        game->ai_paddle.y = temp_ai.y;
    }
    
    check_collisions(game);

    update_powerups(game, effective_delta);

    if (game->ball.x < -50 || game->ball.x > SCREEN_WIDTH + 50) {
        printf("Bola saiu da tela - resetando...\n");
        ball_init(&game->ball, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    }
}

void state_pause_update(Game* game, float delta_time) {
}

void state_victory_update(Game* game, float delta_time) {
    static float victory_timer = 0;
    victory_timer += delta_time;

    if (victory_timer > 5.0f) {
        victory_timer = 0;
        game->current_state = STATE_LEVEL_SELECT;

        if (game->current_level < MAX_LEVELS - 1) {
            game->levels[game->current_level + 1].unlocked = true;
        }

        game->player_paddle.score = 0;
        game->ai_paddle.score = 0;
        audio_play_music(MUSIC_MENU);

        printf("Retornando à seleção de níveis...\n");
    }
}

void state_defeat_update(Game* game, float delta_time) {
    static float defeat_timer = 0;
    defeat_timer += delta_time;

    if (defeat_timer > 3.0f) {
        defeat_timer = 0;
        game->current_state = STATE_LEVEL_SELECT;

        game->player_paddle.score = 0;
        game->ai_paddle.score = 0;
        audio_play_music(MUSIC_MENU);

        printf("Retornando à seleção de níveis...\n");
    }
}


void paddle_update_ai(Paddle* ai_paddle, Ball* ball, int difficulty, float delta_time) {
    if (!ball->active) return;

    float target_y = ball->y - ai_paddle->height / 2;
    float diff = target_y - ai_paddle->y;

    float base_speed = ai_paddle->speed;
    float error_factor = 0.0f;
    float reaction_delay = 0.0f;

    switch (difficulty) {
        case 0: // Fácil
            base_speed *= 0.6f;
            error_factor = 40.0f;
            reaction_delay = 0.5f;
            break;
        case 1: // Médio
            base_speed *= 0.8f;
            error_factor = 20.0f;
            reaction_delay = 0.3f;
            break;
        case 2: // Difícil
            base_speed *= 1.0f;
            error_factor = 5.0f;
            reaction_delay = 0.1f;
            break;
    }

    if (error_factor > 0) {
        float error = (rand() % (int)(error_factor * 2) - error_factor);
        target_y += error;
    }

    static float ai_timer = 0.0f;
    ai_timer += delta_time;
    
    if (ai_timer < reaction_delay) {
        return; // IA não reage ainda
    }
    
    if (ai_timer > 1.0f) {
        ai_timer = 0.0f;
    }

    if (fabs(diff) > 5) {
        float move_speed = base_speed * delta_time;
        if (diff > 0) {
            ai_paddle->y += move_speed;
        } else {
            ai_paddle->y -= move_speed;
        }
    }

    if (ai_paddle->y < 0) ai_paddle->y = 0;
    if (ai_paddle->y > SCREEN_HEIGHT - ai_paddle->height) {
        ai_paddle->y = SCREEN_HEIGHT - ai_paddle->height;
    }
}

void check_collisions(Game* game) {
    Ball* ball = &game->ball;
    CheatSystem* cheat = &game->cheat_system;

    if (game->enemy.barrier.active) {
        if (ball->x + ball->radius >= game->enemy.barrier.x &&
            ball->x - ball->radius <= game->enemy.barrier.x + game->enemy.barrier.width &&
            ball->y + ball->radius >= game->enemy.barrier.y &&
            ball->y - ball->radius <= game->enemy.barrier.y + game->enemy.barrier.height) {
            
            if (ball->vx > 0) {
                ball->vx = -fabs(ball->vx);
                ball->x = game->enemy.barrier.x - ball->radius;
            } else {
                ball->vx = fabs(ball->vx);
                ball->x = game->enemy.barrier.x + game->enemy.barrier.width + ball->radius;
            }
            
            audio_play_sound(SOUND_BALL_HIT);
            printf("Bola rebateu na Muralha de Aço!\n");
        }
    }

    if (ball->x < 0) {
        game->ai_paddle.score++;
        printf("AI marcou! Placar: Jogador %d x %d AI\n", game->player_paddle.score, game->ai_paddle.score);
        
        ball_init(ball, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        
        SDL_Delay(500);
        return;
    }
    
    if (ball->x > SCREEN_WIDTH) {
        game->player_paddle.score++;
        printf("Jogador marcou! Placar: Jogador %d x %d AI\n", game->player_paddle.score, game->ai_paddle.score);
        
        ball_init(ball, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        
        SDL_Delay(500);
        return;
    }

    if (!cheat->noclip_mode) {
        if (ball->y <= ball->radius) {
            ball->y = ball->radius;
            ball_reverse_y(ball);
        }
        if (ball->y >= SCREEN_HEIGHT - ball->radius) {
            ball->y = SCREEN_HEIGHT - ball->radius;
            ball_reverse_y(ball);
        }

        if (ball->x - ball->radius <= game->player_paddle.x + game->player_paddle.width &&
            ball->x + ball->radius >= game->player_paddle.x &&
            ball->y >= game->player_paddle.y &&
            ball->y <= game->player_paddle.y + game->player_paddle.height &&
            ball->vx < 0) {

            ball->vx = fabs(ball->vx);
            ball->x = game->player_paddle.x + game->player_paddle.width + ball->radius;

            float impact_pos = (ball->y - game->player_paddle.y) / game->player_paddle.height;
            float angle_variation = (impact_pos - 0.5f) * 400;
            ball->vy = angle_variation;

            ball_increase_speed(ball, game->levels[game->current_level].ball_speed_multiplier);

            game->total_hits++;
            
            audio_play_sound(SOUND_BALL_HIT);
            printf("Rebatida do jogador! (Total: %d)\n", game->total_hits);
        }

        if (ball->x + ball->radius >= game->ai_paddle.x &&
            ball->x - ball->radius <= game->ai_paddle.x + game->ai_paddle.width &&
            ball->y >= game->ai_paddle.y &&
            ball->y <= game->ai_paddle.y + game->ai_paddle.height &&
            ball->vx > 0) {

            ball->vx = -fabs(ball->vx);
            ball->x = game->ai_paddle.x - ball->radius;

            float impact_pos = (ball->y - game->ai_paddle.y) / game->ai_paddle.height;
            float angle_variation = (impact_pos - 0.5f) * 400;
            ball->vy = angle_variation;

            audio_play_sound(SOUND_BALL_HIT);
            printf("Rebatida da AI!\n");
        }

        if (game->enemy.clone.active) {
            if (ball->x + ball->radius >= game->enemy.clone.x &&
                ball->x - ball->radius <= game->enemy.clone.x + game->enemy.clone.width &&
                ball->y + ball->radius >= game->enemy.clone.y &&
                ball->y - ball->radius <= game->enemy.clone.y + game->enemy.clone.height &&
                ball->vx > 0) {
                
                ball->vx = -fabs(ball->vx) * 1.3f;
                ball->x = game->enemy.clone.x - ball->radius;
                
                float impact_pos = (ball->y - game->enemy.clone.y) / game->enemy.clone.height;
                float angle_variation = (impact_pos - 0.5f) * 400;
                ball->vy += angle_variation;
                
                audio_play_sound(SOUND_BALL_HIT);
                printf("Bola rebateu no Clone das Sombras com força aumentada!\n");
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        PowerUp* powerup = &game->powerups[i];
        if (powerup->active) {
            float dx = ball->x - powerup->x;
            float dy = ball->y - powerup->y;
            float distance = sqrt(dx * dx + dy * dy);
            
            if (distance <= ball->radius + 10) {
                switch (powerup->type) {
                    case 0: // Speed boost
                        ball_increase_speed(ball, 1.2f);
                        printf("Power-up: Speed Boost ativado!\n");
                        break;
                    case 1: // Size increase
                        game->player_paddle.height *= 1.3f;
                        printf("Power-up: Paddle maior ativado!\n");
                        break;
                    case 2: // Multi-ball (simplificado)
                        ball_increase_speed(ball, 1.5f);
                        printf("Power-up: Multi-ball ativado!\n");
                        break;
                }
                powerup->active = false;
            }
        }
    }
}

void update_powerups(Game* game, float delta_time) {
	CheatSystem* cheat = &game->cheat_system;
	
    for (int i = 0; i < 5; i++) {
        PowerUp* powerup = &game->powerups[i];
        if (powerup->active) {
            powerup->timer -= delta_time;
            if (powerup->timer <= 0) {
                powerup->active = false;
                printf("Power-up %d expirou\n", powerup->type);
            }
        }
    }

    static float powerup_spawn_timer = 0;
    powerup_spawn_timer += delta_time;

    float spawn_interval = cheat->infinite_power ? 2.0f : 15.0f; // Spawn mais rápido com cheat
    
    if (powerup_spawn_timer > spawn_interval && game->levels[game->current_level].has_powerups) {
        for (int i = 0; i < 5; i++) {
            if (!game->powerups[i].active) {
                game->powerups[i].x = 200 + (rand() % 400); // Área central
                game->powerups[i].y = 100 + (rand() % 400);
                game->powerups[i].type = rand() % 3; // 0=speed, 1=size, 2=multi
                game->powerups[i].active = true;
                game->powerups[i].timer = 10.0f; // 10 segundos para coletar
                powerup_spawn_timer = 0;
                printf("Power-up spawnou! Tipo: %d na posição (%.0f, %.0f)\n", 
                       game->powerups[i].type, game->powerups[i].x, game->powerups[i].y);
                break;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        PowerUp* powerup = &game->powerups[i];
        if (powerup->active) {
            float dx = game->ball.x - powerup->x;
            float dy = game->ball.y - powerup->y;
            float distance = sqrtf(dx * dx + dy * dy);
            
            if (distance <= game->ball.radius + 15) { // 15 é o raio do power-up
                switch (powerup->type) {
                    case 0: // Speed boost
                        ball_increase_speed(&game->ball, 1.3f);
                        printf("Power-up coletado: Speed Boost!\n");
                        break;
                    case 1: // Size increase
                        game->player_paddle.height *= 1.3f;
                        if (game->player_paddle.height > 150) {
                            game->player_paddle.height = 150; // Limite máximo
                        }
                        printf("Power-up coletado: Paddle maior!\n");
                        break;
                    case 2: // Multi-ball simulation
                        ball_increase_speed(&game->ball, 1.5f);
                        printf("Power-up coletado: Multi-ball!\n");
                        break;
                }
                powerup->active = false;
                
                if (game->sound_enabled) {
                    printf("♪ Som de power-up coletado! ♪\n");
                }
            }
        }
    }
}

void load_game_config(Game* game) {
    fileconfig_load(game->config, "pingoteca_config.txt");

    if (!fileconfig_get_float(game->config, "paddle_sensitivity", &game->paddle_sensitivity)) {
        game->paddle_sensitivity = 1.0f;
    }

    if (!fileconfig_get_bool(game->config, "sound_enabled", &game->sound_enabled)) {
        game->sound_enabled = true;
    }

    if (!fileconfig_get_bool(game->config, "effects_enabled", &game->effects_enabled)) {
        game->effects_enabled = true;
    }
    
    if (!fileconfig_get_float(game->config, "game_speed", &game->game_speed)) {
        game->game_speed = 1.0f;
    }
}

void save_game_config(Game* game) {
    fileconfig_set_float(game->config, "paddle_sensitivity", game->paddle_sensitivity);
    fileconfig_set_bool(game->config, "sound_enabled", game->sound_enabled);
    fileconfig_set_bool(game->config, "effects_enabled", game->effects_enabled);
    fileconfig_set_int(game->config, "total_hits", game->total_hits);
    fileconfig_set_int(game->config, "coins_earned", game->coins_earned);
    fileconfig_set_float(game->config, "game_speed", game->game_speed); // NOVO

    fileconfig_save(game->config, "pingoteca_config.txt");
}

void init_levels(Game* game) {
    strcpy(game->levels[0].name, "Primeira Rebatida");
    strcpy(game->levels[0].theme, "Clássico");
    game->levels[0].ball_speed_multiplier = 1.0f;
    game->levels[0].ai_difficulty = 1;
    game->levels[0].unlocked = true;
    game->levels[0].completed = false;
    game->levels[0].has_powerups = false;
    game->levels[0].has_obstacles = false;

    strcpy(game->levels[1].name, "Neblina Lunar");
    strcpy(game->levels[1].theme, "Nebuloso");
    game->levels[1].ball_speed_multiplier = 1.2f;
    game->levels[1].ai_difficulty = 2;
    game->levels[1].unlocked = false;
    game->levels[1].completed = false;
    game->levels[1].has_powerups = true;
    game->levels[1].has_obstacles = false;

    strcpy(game->levels[2].name, "Caos Elétrico");
    strcpy(game->levels[2].theme, "Elétrico");
    game->levels[2].ball_speed_multiplier = 1.5f;
    game->levels[2].ai_difficulty = 3;
    game->levels[2].unlocked = false;
    game->levels[2].completed = false;
    game->levels[2].has_powerups = true;
    game->levels[2].has_obstacles = true;

    for (int i = 3; i < MAX_LEVELS; i++) {
        snprintf(game->levels[i].name, sizeof(game->levels[i].name), "Nível %d", i + 1);
        strcpy(game->levels[i].theme, "Avançado");
        game->levels[i].ball_speed_multiplier = 1.0f + 0.3f * i;
        game->levels[i].ai_difficulty = (i / 2) + 1;
        game->levels[i].unlocked = false;
        game->levels[i].completed = false;
        game->levels[i].has_powerups = true;
        game->levels[i].has_obstacles = i > 4;
    }
}

float get_time_seconds(void) {
    return (float)clock() / CLOCKS_PER_SEC;
}
