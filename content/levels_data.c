#include "levels_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEVELS 9

const LevelData* levels_get_data(int level_id) {
    if (level_id < 1 || level_id > MAX_LEVELS) {
        printf("Erro: ID de nível inválido: %d\n", level_id);
        return NULL;
    }

    return &LEVELS_DATA[level_id - 1];
}

const BossData* levels_get_boss_data(int level_id) {
    const LevelData* level = levels_get_data(level_id);
    if (!level || level->type != LEVEL_TYPE_BOSS) {
        return NULL;
    }

    for (int i = 0; i < 3; i++) { // Temos 3 bosses definidos
        if (BOSS_DATA[i].level_id == level_id) {
            return &BOSS_DATA[i];
        }
    }

    printf("Aviso: Dados de boss não encontrados para nível %d\n", level_id);
    return NULL;
}

bool levels_is_boss_level(int level_id) {
    const LevelData* level = levels_get_data(level_id);
    return (level && level->type == LEVEL_TYPE_BOSS);
}

int levels_get_required_score(int level_id) {
    const LevelData* level = levels_get_data(level_id);
    return level ? level->required_score_to_unlock : 0;
}

int levels_get_max_score(int level_id) {
    const LevelData* level = levels_get_data(level_id);
    return level ? level->max_score : 0;
}

int levels_count(void) {
    return MAX_LEVELS;
}

int levels_get_completed_count(void) {
    int completed = 0;

    for (int i = 0; i < MAX_LEVELS; i++) {
        if (LEVELS_DATA[i].completed) {
            completed++;
        }
    }

    return completed;
}

int levels_get_total_stars(void) {
    int total_stars = 0;

    for (int i = 0; i < MAX_LEVELS; i++) {
        total_stars += LEVELS_DATA[i].stars_earned;
    }

    return total_stars;
}

int levels_calculate_stars(int level_id, int score, float time) {
    const LevelData* level = levels_get_data(level_id);
    if (!level) return 0;

    int stars = 1; // Sempre ganha pelo menos 1 estrela ao completar

    if (score >= (level->max_score * 3) / 4) {
        stars = 2;
    }

    if (score >= level->max_score) {
        stars = 3;
    }

    if (level->time_limit > 0 && time > 0.0f) {
        if (time <= level->time_limit * 0.75f && stars >= 2) {
            stars = 3;
        }
    }

    return stars;
}



bool levels_load_progress(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Aviso: Arquivo de progresso %s não encontrado, usando dados padrão\n", filename);
        return false;
    }

    for (int i = 0; i < MAX_LEVELS; i++) {
        struct {
            int id;
            int stars_earned;
            bool completed;
            int best_score;
            float best_time;
        } save_data;

        if (fread(&save_data, sizeof(save_data), 1, file) != 1) {
            printf("Erro: Falha ao ler dados do nível %d\n", i + 1);
            fclose(file);
            return false;
        }

        if (save_data.id != i + 1) {
            printf("Erro: ID de nível inconsistente no arquivo de save\n");
            fclose(file);
            return false;
        }

        LevelData* level = (LevelData*)&LEVELS_DATA[i];
        level->stars_earned = save_data.stars_earned;
        level->completed = save_data.completed;
        level->best_score = save_data.best_score;
        level->best_time = save_data.best_time;
    }

    fclose(file);
    printf("Progresso carregado de %s\n", filename);
    return true;
}

void levels_reset_progress(void) {
    for (int i = 0; i < MAX_LEVELS; i++) {
        LevelData* level = (LevelData*)&LEVELS_DATA[i];
        level->stars_earned = 0;
        level->completed = false;
        level->best_score = 0;
        level->best_time = 0.0f;
    }

    printf("Progresso dos níveis resetado\n");
}

void levels_debug_print_all(void) {
    printf("\n=== DEBUG: Dados dos Níveis ===\n");

    for (int i = 0; i < MAX_LEVELS; i++) {
        const LevelData* level = &LEVELS_DATA[i];

        printf("Nível %d: %s\n", level->id, level->name);
        printf("  Descrição: %s\n", level->description);
        printf("  Tipo: %s\n", level->type == LEVEL_TYPE_BOSS ? "Boss" : "Normal");
        printf("  Completado: %s\n", level->completed ? "Sim" : "Não");
        printf("  Estrelas: %d/3\n", level->stars_earned);
        printf("  Melhor pontuação: %d/%d\n", level->best_score, level->max_score);

        if (level->best_time > 0.0f) {
            printf("  Melhor tempo: %.2fs\n", level->best_time);
        }

        printf("  ---\n");
    }

    printf("Total de níveis completados: %d/%d\n", levels_get_completed_count(), MAX_LEVELS);
    printf("Total de estrelas: %d/%d\n", levels_get_total_stars(), MAX_LEVELS * 3);
    printf("==============================\n\n");
}
