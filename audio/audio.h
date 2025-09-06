#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

typedef enum {
    SOUND_NAVIGATION = 0,   // Som de navegação (trocar opção)
    SOUND_SELECT = 1,       // Som de seleção (clicar)
    SOUND_BALL_HIT = 2,     // Som da bola batendo nas raquetes
    SOUND_COUNT = 3
} SoundType;

typedef enum {
    MUSIC_MENU = 0,         // Música do menu
    MUSIC_GAMEPLAY = 1,     // Música do gameplay (baixa)
    MUSIC_BOSS = 2,         // Música de boss fight
    MUSIC_COUNT = 3
} MusicType;

bool audio_init(void);
void audio_cleanup(void);
void audio_play_sound(SoundType sound);
void audio_play_music(MusicType music);
void audio_stop_music(void);
void audio_set_enabled(bool enabled);
bool audio_is_enabled(void);

#endif
