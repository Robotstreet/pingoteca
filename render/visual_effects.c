#include "visual_effects.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void visual_effects_init(VisualEffectsSystem* system) {
    if (!system) return;

    memset(system, 0, sizeof(VisualEffectsSystem));

    system->fog_density = 0.5f;
    system->shake_intensity = 1.0f;
    system->strobe_frequency = 2.0f;
    system->particle_count = 50.0f;
}

void visual_effects_update(VisualEffectsSystem* system, float delta_time) {
    if (!system) return;

    if (system->fog_active) {
        system->fog_timer += delta_time;
        system->fog_density = 0.3f + 0.3f * sinf(system->fog_timer * 0.8f);
    }

    if (system->screen_shake_active) {
        system->shake_timer += delta_time;
        if (system->shake_timer > 0.5f) {
            system->shake_intensity *= 0.9f;
            if (system->shake_intensity < 0.1f) {
                system->screen_shake_active = false;
                system->shake_intensity = 1.0f;
                system->shake_timer = 0.0f;
            }
        }
    }

    if (system->color_distortion_active) {
        static float distortion_phase = 0.0f;
        distortion_phase += delta_time * 2.0f;
        if (distortion_phase > 6.28318f) { // 2 * PI
            distortion_phase = 0.0f;
        }
    }

    if (system->strobe_active) {
        system->strobe_timer += delta_time;
        if (system->strobe_timer >= 1.0f / system->strobe_frequency) {
            system->strobe_timer = 0.0f;
        }
    }

    if (system->particle_effects_active) {
        system->particle_timer += delta_time;
        if (system->particle_timer > 0.1f) {
            system->particle_timer = 0.0f;
        }
    }
}

void visual_effects_render(VisualEffectsSystem* system) {
    if (!system) return;


    static int frame_count = 0;
    frame_count++;

    if (frame_count % 60 == 0) {
        printf("=== VISUAL EFFECTS STATUS ===\n");

        if (system->fog_active) {
            printf("FOG: Active (density: %.2f)\n", system->fog_density);
        }

        if (system->screen_shake_active) {
            printf("SHAKE: Active (intensity: %.2f)\n", system->shake_intensity);
        }

        if (system->color_distortion_active) {
            printf("COLOR DISTORTION: Active\n");
        }

        if (system->strobe_active) {
            printf("STROBE: Active (frequency: %.2f Hz)\n", system->strobe_frequency);
        }

        if (system->particle_effects_active) {
            printf("PARTICLES: Active (count: %.0f)\n", system->particle_count);
        }

        if (!system->fog_active && !system->screen_shake_active &&
            !system->color_distortion_active && !system->strobe_active &&
            !system->particle_effects_active) {
            printf("No visual effects active\n");
        }

        printf("=============================\n");
    }


    if (system->fog_active) {
    }

    if (system->screen_shake_active) {
    }

    if (system->color_distortion_active) {
    }

    if (system->strobe_active) {
    }

    if (system->particle_effects_active) {
    }
}


void visual_effects_activate_fog(VisualEffectsSystem* system, float density) {
    if (!system) return;
    system->fog_active = true;
    system->fog_density = density;
    system->fog_timer = 0.0f;
}

void visual_effects_deactivate_fog(VisualEffectsSystem* system) {
    if (!system) return;
    system->fog_active = false;
    system->fog_timer = 0.0f;
}

void visual_effects_activate_shake(VisualEffectsSystem* system, float intensity) {
    if (!system) return;
    system->screen_shake_active = true;
    system->shake_intensity = intensity;
    system->shake_timer = 0.0f;
}

void visual_effects_deactivate_shake(VisualEffectsSystem* system) {
    if (!system) return;
    system->screen_shake_active = false;
    system->shake_timer = 0.0f;
}

void visual_effects_activate_color_distortion(VisualEffectsSystem* system) {
    if (!system) return;
    system->color_distortion_active = true;
}

void visual_effects_deactivate_color_distortion(VisualEffectsSystem* system) {
    if (!system) return;
    system->color_distortion_active = false;
}

void visual_effects_activate_strobe(VisualEffectsSystem* system, float frequency) {
    if (!system) return;
    system->strobe_active = true;
    system->strobe_frequency = frequency;
    system->strobe_timer = 0.0f;
}

void visual_effects_deactivate_strobe(VisualEffectsSystem* system) {
    if (!system) return;
    system->strobe_active = false;
    system->strobe_timer = 0.0f;
}

void visual_effects_activate_particles(VisualEffectsSystem* system, float count) {
    if (!system) return;
    system->particle_effects_active = true;
    system->particle_count = count;
    system->particle_timer = 0.0f;
}

void visual_effects_deactivate_particles(VisualEffectsSystem* system) {
    if (!system) return;
    system->particle_effects_active = false;
    system->particle_timer = 0.0f;
}

void visual_effects_deactivate_all(VisualEffectsSystem* system) {
    if (!system) return;

    visual_effects_deactivate_fog(system);
    visual_effects_deactivate_shake(system);
    visual_effects_deactivate_color_distortion(system);
    visual_effects_deactivate_strobe(system);
    visual_effects_deactivate_particles(system);
}
