#include "audio.h"
#include <stdio.h>

static Mix_Music* music_tracks[MUSIC_COUNT];
static Mix_Chunk* sounds[SOUND_COUNT];
static bool audio_enabled = true;
static bool audio_initialized = false;
static MusicType current_music = -1;

bool audio_init(void) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Erro ao inicializar SDL_mixer: %s\n", Mix_GetError());
        return false;
    }

    for (int i = 0; i < MUSIC_COUNT; i++) {
        music_tracks[i] = NULL;
    }
    for (int i = 0; i < SOUND_COUNT; i++) {
        sounds[i] = NULL;
    }

    music_tracks[MUSIC_MENU] = Mix_LoadMUS("assets/audio/music/menu.mp3");
    if (!music_tracks[MUSIC_MENU]) {
        printf("Erro ao carregar música do menu: %s\n", Mix_GetError());
        music_tracks[MUSIC_MENU] = Mix_LoadMUS("menu.wav");
        if (!music_tracks[MUSIC_MENU]) {
            printf("Música do menu não encontrada\n");
        }
    }

    music_tracks[MUSIC_GAMEPLAY] = Mix_LoadMUS("assets/audio/music/gameplay.mp3");
    if (!music_tracks[MUSIC_GAMEPLAY]) {
        printf("Erro ao carregar música do gameplay: %s\n", Mix_GetError());
        music_tracks[MUSIC_GAMEPLAY] = Mix_LoadMUS("gameplay.wav");
        if (!music_tracks[MUSIC_GAMEPLAY]) {
            printf("Música do gameplay não encontrada\n");
        }
    }

    music_tracks[MUSIC_BOSS] = Mix_LoadMUS("assets/audio/music/boss.mp3");
    if (!music_tracks[MUSIC_BOSS]) {
        printf("Erro ao carregar música do boss: %s\n", Mix_GetError());
        music_tracks[MUSIC_BOSS] = Mix_LoadMUS("boss.wav");
        if (!music_tracks[MUSIC_BOSS]) {
            printf("Música do boss não encontrada\n");
        }
    }

    sounds[SOUND_NAVIGATION] = Mix_LoadWAV("assets/audio/sfx/navigation.wav");
    if (!sounds[SOUND_NAVIGATION]) {
        printf("Erro ao carregar som de navegação: %s\n", Mix_GetError());
        sounds[SOUND_NAVIGATION] = Mix_LoadWAV("navigation.wav");
        if (!sounds[SOUND_NAVIGATION]) {
            printf("Som de navegação não encontrado\n");
        }
    }

    sounds[SOUND_SELECT] = Mix_LoadWAV("assets/audio/sfx/select.wav");
    if (!sounds[SOUND_SELECT]) {
        printf("Erro ao carregar som de seleção: %s\n", Mix_GetError());
        sounds[SOUND_SELECT] = Mix_LoadWAV("select.wav");
        if (!sounds[SOUND_SELECT]) {
            printf("Som de seleção não encontrado\n");
        }
    }

    sounds[SOUND_BALL_HIT] = Mix_LoadWAV("assets/audio/sfx/ball_hit.wav");
    if (!sounds[SOUND_BALL_HIT]) {
        printf("Erro ao carregar som da batida da bola: %s\n", Mix_GetError());
        sounds[SOUND_BALL_HIT] = Mix_LoadWAV("ball_hit.wav");
        if (!sounds[SOUND_BALL_HIT]) {
            printf("Som da batida da bola não encontrado\n");
        }
    }

    audio_initialized = true;
    
    audio_play_music(MUSIC_MENU);

    printf("Sistema de áudio inicializado.\n");
    return true;
}

void audio_cleanup(void) {
    if (!audio_initialized) return;

    Mix_HaltMusic();
    
    for (int i = 0; i < MUSIC_COUNT; i++) {
        if (music_tracks[i]) {
            Mix_FreeMusic(music_tracks[i]);
            music_tracks[i] = NULL;
        }
    }

    for (int i = 0; i < SOUND_COUNT; i++) {
        if (sounds[i]) {
            Mix_FreeChunk(sounds[i]);
            sounds[i] = NULL;
        }
    }

    Mix_CloseAudio();
    audio_initialized = false;
    current_music = -1;
    printf("Sistema de áudio finalizado.\n");
}

void audio_play_sound(SoundType sound) {
    if (!audio_initialized || !audio_enabled || sound >= SOUND_COUNT || !sounds[sound]) {
        return;
    }

    if (Mix_PlayChannel(-1, sounds[sound], 0) == -1) {
        printf("Erro ao tocar som %d: %s\n", sound, Mix_GetError());
    }
}

void audio_play_music(MusicType music) {
    if (!audio_initialized || !audio_enabled || music >= MUSIC_COUNT || !music_tracks[music]) {
        return;
    }

    if (Mix_PlayingMusic()) {
        Mix_HaltMusic();
    }

    switch (music) {
        case MUSIC_MENU:
            Mix_VolumeMusic(MIX_MAX_VOLUME * 0.7); // 70% para menu
            break;
        case MUSIC_GAMEPLAY:
            Mix_VolumeMusic(MIX_MAX_VOLUME * 0.3); // 30% para gameplay (baixo)
            break;
        case MUSIC_BOSS:
            Mix_VolumeMusic(MIX_MAX_VOLUME * 0.8); // 80% para boss fight
            break;
    }

    if (Mix_PlayMusic(music_tracks[music], -1) == -1) {
        printf("Erro ao tocar música %d: %s\n", music, Mix_GetError());
    } else {
        current_music = music;
        const char* music_names[] = {"MENU", "GAMEPLAY", "BOSS"};
        printf("Música %s iniciada\n", music_names[music]);
    }
}

void audio_stop_music(void) {
    if (!audio_initialized) return;
    
    Mix_HaltMusic();
    current_music = -1;
    printf("Música parada\n");
}

void audio_set_enabled(bool enabled) {
    audio_enabled = enabled;
    
    if (!audio_initialized) return;

    if (!enabled) {
        Mix_HaltMusic();
        Mix_HaltChannel(-1); // Para todos os efeitos sonoros
    } else if (current_music != -1) {
        audio_play_music(current_music);
    }
    
    printf("Som %s\n", enabled ? "ATIVADO" : "DESATIVADO");
}

bool audio_is_enabled(void) {
    return audio_enabled;
}
