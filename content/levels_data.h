#ifndef LEVELS_DATA_H
#define LEVELS_DATA_H

#include <stdbool.h>

#define MAX_LEVELS 9

typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD,
    DIFFICULTY_EXPERT,
    DIFFICULTY_BOSS
} LevelDifficulty;

typedef enum {
    LEVEL_TYPE_NORMAL,
    LEVEL_TYPE_BOSS,
    LEVEL_TYPE_BONUS,
    LEVEL_TYPE_SECRET
} LevelType;

typedef struct {
    int id;
    const char* name;
    const char* description;
    LevelType type;
    LevelDifficulty difficulty;
    float ball_speed_multiplier;
    float ai_reaction_time;
    float ai_accuracy;
    int max_score;
    int time_limit;  // 0 = no time limit
    bool has_power_ups;
    bool has_obstacles;
    bool unlocked_by_default;
    int required_level_to_unlock;
    int required_score_to_unlock;
    int stars_earned;  // 0-3 stars
    bool completed;
    int best_score;
    float best_time;
    const char* background_texture;
    const char* music_track;
    const char* ambient_sound;
    float floor_color_r, floor_color_g, floor_color_b;
    float accent_color_r, accent_color_g, accent_color_b;
    
} LevelData;

static const LevelData LEVELS_DATA[MAX_LEVELS] = {
    {
        .id = 1,
        .name = "Neblina Lunar",
        .description = "Uma partida suave sob a luz da lua nebulosa",
        .type = LEVEL_TYPE_NORMAL,
        .difficulty = DIFFICULTY_EASY,
        .ball_speed_multiplier = 0.8f,
        .ai_reaction_time = 0.5f,
        .ai_accuracy = 0.6f,
        .max_score = 5,
        .time_limit = 0,
        .has_power_ups = true,
        .has_obstacles = false,
        .unlocked_by_default = true,
        .required_level_to_unlock = 0,
        .required_score_to_unlock = 0,
        .stars_earned = 0,
        .completed = false,
        .best_score = 0,
        .best_time = 0.0f,
        .background_texture = "level_1_neblina_lunar",
        .music_track = "level_1_neblina_lunar",
        .ambient_sound = "wind",
        .floor_color_r = 0.2f, .floor_color_g = 0.3f, .floor_color_b = 0.6f,
        .accent_color_r = 0.4f, .accent_color_g = 0.5f, .accent_color_b = 0.8f
    },

    {
        .id = 2,
        .name = "Caos Elétrico",
        .description = "Raios cortam o campo, energia pura no ar",
        .type = LEVEL_TYPE_NORMAL,
        .difficulty = DIFFICULTY_NORMAL,
        .ball_speed_multiplier = 1.0f,
        .ai_reaction_time = 0.4f,
        .ai_accuracy = 0.7f,
        .max_score = 7,
        .time_limit = 0,
        .has_power_ups = true,
        .has_obstacles = true,
        .unlocked_by_default = false,
        .required_level_to_unlock = 1,
        .required_score_to_unlock = 5,
        .stars_earned = 0,
        .completed = false,
        .best_score = 0,
        .best_time = 0.0f,
        .background_texture = "level_2_caos_eletrico",
        .music_track = "level_2_caos_eletrico",
        .ambient_sound = "electric",
        .floor_color_r = 0.4f, .floor_color_g = 0.1f, .floor_color_b = 0.7f,
        .accent_color_r = 0.7f, .accent_color_g = 0.3f, .accent_color_b = 1.0f
    },

    {
        .id = 3,
        .name = "Fogo Eterno",
        .description = "Chamas dançam, o calor intensifica o jogo",
        .type = LEVEL_TYPE_BOSS,
        .difficulty = DIFFICULTY_HARD,
        .ball_speed_multiplier = 1.2f,
        .ai_reaction_time = 0.3f,
        .ai_accuracy = 0.8f,
        .max_score = 10,
        .time_limit = 300,  // 5 minutes
        .has_power_ups = true,
        .has_obstacles = true,
        .unlocked_by_default = false,
        .required_level_to_unlock = 2,
        .required_score_to_unlock = 7,
        .stars_earned = 0,
        .completed = false,
        .best_score = 0,
        .best_time = 0.0f,
        .background_texture = "level_3_fogo_eterno",
        .music_track = "level_3_fogo_eterno",
        .ambient_sound = "fire",
        .floor_color_r = 0.8f, .floor_color_g = 0.2f, .floor_color_b = 0.1f,
        .accent_color_r = 1.0f, .accent_color_g = 0.5f, .accent_color_b = 0.0f
    },

    {
        .id = 4,
        .name = "Gelo Ártico",
        .description = "O frio congela até os reflexos mais rápidos",
        .type = LEVEL_TYPE_NORMAL,
        .difficulty = DIFFICULTY_NORMAL,
        .ball_speed_multiplier = 0.9f,
        .ai_reaction_time = 0.45f,
        .ai_accuracy = 0.75f,
        .max_score = 8,
        .time_limit = 0,
        .has_power_ups = true,
        .has_obstacles = true,
        .unlocked_by_default = false,
        .required_level_to_unlock = 3,
        .required_score_to_unlock = 10,
        .stars_earned = 0,
        .completed = false,
        .best_score = 0,
        .best_time = 0.0f,
        .background_texture = "level_4_gelo_artico",
        .music_track = "level_4_gelo_artico",
        .ambient_sound = "ice",
        .floor_color_r = 0.6f, .floor_color_g = 0.8f, .floor_color_b = 1.0f,
        .accent_color_r = 0.8f, .accent_color_g = 0.9f, .accent_color_b = 1.0f
    },

    {
        .id = 5,
        .name = "Floresta Mística",
        .description = "Magia antiga permeia cada rebatida",
        .type = LEVEL_TYPE_NORMAL,
        .difficulty = DIFFICULTY_HARD,
        .ball_speed_multiplier = 1.1f,
        .ai_reaction_time = 0.35f,
        .ai_accuracy = 0.8f,
        .max_score = 9,
        .time_limit = 0,
        .has_power_ups = true,
        .has_obstacles = true,
        .unlocked_by_default = false,
        .required_level_to_unlock = 4,
        .required_score_to_unlock = 8,
        .stars_earned = 0,
        .completed = false,
        .best_score = 0,
        .best_time = 0.0f,
        .background_texture = "level_5_floresta_mistica",
        .music_track = "level_5_floresta_mistica",
        .ambient_sound = "wind",
        .floor_color_r = 0.2f, .floor_color_g = 0.6f, .floor_color_b = 0.2f,
        .accent_color_r = 0.4f, .accent_color_g = 0.8f, .accent_color_b = 0.3f
    },

    {
        .id = 6,
        .name = "Tempestade",
        .description = "Trovões ecoam, a bola dança com o vento",
        .type = LEVEL_TYPE_BOSS,
        .difficulty = DIFFICULTY_HARD,
        .ball_speed_multiplier = 1.3f,
        .ai_reaction_time = 0.25f,
        .ai_accuracy = 0.85f,
        .max_score = 12,
        .time_limit = 240,  // 4 minutes
        .has_power_ups = true,
        .has_obstacles = true,
        .unlocked_by_default = false,
        .required_level_to_unlock = 5,
        .required_score_to_unlock = 9,
        .stars_earned = 0,
        .completed = false,
        .best_score = 0,
        .best_time = 0.0f,
        .background_texture = "level_6_tempestade",
        .music_track = "level_6_tempestade",
        .ambient_sound = "thunder",
        .floor_color_r = 0.3f, .floor_color_g = 0.3f, .floor_color_b = 0.4f,
        .accent_color_r = 0.5f, .accent_color_g = 0.5f, .accent_color_b = 0.7f
    },

    {
        .id = 7,
        .name = "Vulcão",
        .description = "Lava borbulha, cada erro pode ser fatal",
        .type = LEVEL_TYPE_NORMAL,
        .difficulty = DIFFICULTY_EXPERT,
        .ball_speed_multiplier = 1.4f,
        .ai_reaction_time = 0.2f,
        .ai_accuracy = 0.9f,
        .max_score = 10,
        .time_limit = 0,
        .has_power_ups = true,
        .has_obstacles = true,
        .unlocked_by_default = false,
        .required_level_to_unlock = 6,
        .required_score_to_unlock = 12,
        .stars_earned = 0,
        .completed = false,
        .best_score = 0,
        .best_time = 0.0f,
        .background_texture = "level_7_vulcao",
        .music_track = "level_7_vulcao",
        .ambient_sound = "fire",
        .floor_color_r = 0.6f, .floor_color_g = 0.1f, .floor_color_b = 0.0f,
        .accent_color_r = 1.0f, .accent_color_g = 0.3f, .accent_color_b = 0.0f
    },

    {
        .id = 8,
        .name = "Espaço Sideral",
        .description = "Gravidade zero, física impossível",
        .type = LEVEL_TYPE_NORMAL,
        .difficulty = DIFFICULTY_EXPERT,
        .ball_speed_multiplier = 1.5f,
        .ai_reaction_time = 0.15f,
        .ai_accuracy = 0.95f,
        .max_score = 11,
        .time_limit = 0,
        .has_power_ups = true,
        .has_obstacles = true,
        .unlocked_by_default = false,
        .required_level_to_unlock = 7,
        .required_score_to_unlock = 10,
        .stars_earned = 0,
        .completed = false,
        .best_score = 0,
        .best_time = 0.0f,
        .background_texture = "level_8_espaco_sideral",
        .music_track = "level_8_espaco_sideral",
        .ambient_sound = "space",
        .floor_color_r = 0.1f, .floor_color_g = 0.0f, .floor_color_b = 0.3f,
        .accent_color_r = 0.3f, .accent_color_g = 0.1f, .accent_color_b = 0.6f
    },

    {
        .id = 9,
        .name = "Dimensão Final",
        .description = "O último desafio, onde apenas mestres sobrevivem",
        .type = LEVEL_TYPE_BOSS,
        .difficulty = DIFFICULTY_EXPERT,
        .ball_speed_multiplier = 1.8f,
        .ai_reaction_time = 0.1f,
        .ai_accuracy = 1.0f,
        .max_score = 15,
        .time_limit = 180,  // 3 minutes
        .has_power_ups = true,
        .has_obstacles = true,
        .unlocked_by_default = false,
        .required_level_to_unlock = 8,
        .required_score_to_unlock = 11,
        .stars_earned = 0,
        .completed = false,
        .best_score = 0,
        .best_time = 0.0f,
        .background_texture = "level_9_dimensao_final",
        .music_track = "level_9_dimensao_final",
        .ambient_sound = "space",
        .floor_color_r = 0.6f, .floor_color_g = 0.5f, .floor_color_b = 0.1f,
        .accent_color_r = 1.0f, .accent_color_g = 0.8f, .accent_color_b = 0.2f
    }
};

typedef struct {
    int level_id;
    const char* name;
    const char* description;
    float health;
    float speed_multiplier;
    float attack_frequency;
    float special_attack_frequency;
    const char* texture;
    const char* attack_sound;
    const char* defeat_sound;
} BossData;

static const BossData BOSS_DATA[] = {
    {
        .level_id = 3,
        .name = "Raquete Furiosa",
        .description = "Uma raquete incandescente que nunca erra",
        .health = 100.0f,
        .speed_multiplier = 1.5f,
        .attack_frequency = 2.0f,
        .special_attack_frequency = 5.0f,
        .texture = "boss_raquete_furiosa",
        .attack_sound = "boss_raquete_furiosa",
        .defeat_sound = "boss_defeat"
    },

    {
        .level_id = 6,
        .name = "Guardião do Gelo",
        .description = "Controlador das tempestades geladas",
        .health = 150.0f,
        .speed_multiplier = 1.3f,
        .attack_frequency = 1.5f,
        .special_attack_frequency = 4.0f,
        .texture = "boss_guardiao_gelo",
        .attack_sound = "boss_guardiao_gelo",
        .defeat_sound = "boss_defeat"
    },

    {
        .level_id = 9,
        .name = "Mestre Final",
        .description = "O último desafio da dimensão",
        .health = 200.0f,
        .speed_multiplier = 2.0f,
        .attack_frequency = 1.0f,
        .special_attack_frequency = 3.0f,
        .texture = "boss_mestre_final",
        .attack_sound = "boss_mestre_final",
        .defeat_sound = "boss_defeat"
    }
};

const LevelData* levels_get_data(int level_id);
const BossData* levels_get_boss_data(int level_id);
bool levels_is_unlocked(int level_id);
bool levels_is_boss_level(int level_id);
int levels_get_required_score(int level_id);
int levels_get_max_score(int level_id);
const char* levels_get_difficulty_name(LevelDifficulty difficulty);
int levels_count(void);
int levels_get_completed_count(void);
int levels_get_total_stars(void);

bool levels_complete_level(int level_id, int score, float time);
int levels_calculate_stars(int level_id, int score, float time);

bool levels_save_progress(const char* filename);
bool levels_load_progress(const char* filename);
void levels_reset_progress(void);

void levels_debug_print_all(void);

#endif // LEVELS_DATA_H
