#ifndef VISUAL_EFFECTS_H
#define VISUAL_EFFECTS_H

#include <stdbool.h>

typedef struct {
    bool fog_active;
    bool screen_shake_active;
    bool color_distortion_active;
    bool strobe_active;
    bool particle_effects_active;
    
    float fog_density;
    float shake_intensity;
    float strobe_frequency;
    float particle_count;
    
    float fog_timer;
    float shake_timer;
    float strobe_timer;
    float particle_timer;
} VisualEffectsSystem;

void visual_effects_init(VisualEffectsSystem* system);
void visual_effects_update(VisualEffectsSystem* system, float delta_time);
void visual_effects_render(VisualEffectsSystem* system);

void visual_effects_activate_fog(VisualEffectsSystem* system, float density);
void visual_effects_deactivate_fog(VisualEffectsSystem* system);
void visual_effects_activate_shake(VisualEffectsSystem* system, float intensity);
void visual_effects_deactivate_shake(VisualEffectsSystem* system);
void visual_effects_activate_color_distortion(VisualEffectsSystem* system);
void visual_effects_deactivate_color_distortion(VisualEffectsSystem* system);
void visual_effects_activate_strobe(VisualEffectsSystem* system, float frequency);
void visual_effects_deactivate_strobe(VisualEffectsSystem* system);
void visual_effects_activate_particles(VisualEffectsSystem* system, float count);
void visual_effects_deactivate_particles(VisualEffectsSystem* system);
void visual_effects_deactivate_all(VisualEffectsSystem* system);

#endif // VISUAL_EFFECTS_H
