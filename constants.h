#pragma once

#define WINDOW_SIZE 1000
#define WINDOW_TITLE "Charged particles interaction simulation"
#define CLEAR_COLOR 0.07f, 0.13f, 0.17f, 1.0f
#define CIRCLE_DRAW_STEPS 50
#define CIRCLE_RADIUS_PER_MASS_UNIT 0.05f // The window is 2x2. The radius is in that proportion (so 1 radius = the whole screen).
#define PARTICLE_CHARGE_MULTIPLIER 0.00001 // used to simplify code, such as choosing the charge of the placeholder particle
#define PARTICLE_MIN_CHARGE -0.00001f
#define PARTICLE_MAX_CHARGE 0.00001f
#define PARTICLE_MIN_MASS 0.1f
#define PARTICLE_MAX_MASS 2.0f
#define PARTICLE_MASS_INCREMENT_AMOUNT 0.2f
#define PARTICLE_CHARGE_INCREMENT_AMOUNT 0.2f * PARTICLE_CHARGE_MULTIPLIER
#define PARTICLE_DRAG_DROP_VELOCITY_MULTIPLIER 3