#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config g_config = {0};

void config_init(void) {
    printf("Inicializando configurações...\n");

    config_reset_to_default();

    config_load();

    printf("Configurações inicializadas.\n");
}

void config_load(void) {
    FILE* file = fopen("config.dat", "rb");

    if (!file) {
        printf("Arquivo de configuração não encontrado, usando padrões.\n");
        return;
    }

    size_t read = fread(&g_config, sizeof(Config), 1, file);
    fclose(file);

    if (read != 1) {
        printf("Erro ao ler configurações, restaurando padrões.\n");
        config_reset_to_default();
        return;
    }

    printf("Configurações carregadas com sucesso.\n");
}

void config_save(void) {
    FILE* file = fopen("config.dat", "wb");

    if (!file) {
        printf("Erro ao salvar configurações.\n");
        return;
    }

    size_t written = fwrite(&g_config, sizeof(Config), 1, file);
    fclose(file);

    if (written == 1) {
        printf("Configurações salvas com sucesso.\n");
    } else {
        printf("Erro ao escrever configurações.\n");
    }
}

void config_reset_to_default(void) {
    printf("Restaurando configurações padrão...\n");

    g_config.fullscreen = false;
    g_config.vsync = true;
    g_config.show_fps = false;
    g_config.debug_mode = false;

    g_config.master_volume = MASTER_VOLUME;
    g_config.music_volume = MUSIC_VOLUME;
    g_config.sfx_volume = SFX_VOLUME;

    g_config.difficulty = 1; // Normal

    printf("Configurações padrão aplicadas.\n");
}
