cmake_minimum_required(VERSION 3.7)
project(Pingoteca C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Encontrar bibliotecas
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)

# Fontes
set(CORE_SRC
    core/engine.c
    core/states.c
    core/config.c
)
set(RENDER_SRC
    render/visual_effects.c
)
set(AUDIO_SRC
    audio/audio.c
)
set(INPUT_SRC
)
set(PHYSICS_SRC
    physics/ball.c
    physics/paddle.c
)
set(GAME_SRC
    game/enemy.c
    game/powerups.c
)
set(UI_SRC
    ui/text_renderer.c
    ui/dialog.c
    ui/cheat_system.c
)
set(DATA_SRC
)
set(AI_SRC
)
set(UTIL_SRC
    utils/file_utils.c
    utils/string_utils.c
)
set(CONTENT_SRC
    content/levels_data.c
)

# Criar executável
add_executable(pingoteca
    main.c
    ${CORE_SRC}
    ${RENDER_SRC}
    ${AUDIO_SRC}
    ${INPUT_SRC}
    ${PHYSICS_SRC}
    ${GAME_SRC}
    ${UI_SRC}
    ${DATA_SRC}
    ${AI_SRC}
    ${UTIL_SRC}
    ${CONTENT_SRC}
)

# Linkar usando targets modernos
target_link_libraries(pingoteca
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_mixer::SDL2_mixer
    SDL2_image::SDL2_image
    m
)
