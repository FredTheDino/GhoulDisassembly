#include "util.h"
#include <math.h>

Vec2 vec_form_angle(f32 angle) {
    return fog_V2(cos(angle), sin(angle));
}

f32 rotate_towards(f32 speed, f32 a, f32 b) {
    // Shortest rotation
    f32 delta = b - a;
    if (delta < M_PI)
        delta += 2 * M_PI;
    if (delta > M_PI)
        delta -= 2 * M_PI;

    if (abs(delta) < speed)
        return b;
    if (delta < 0)
        return a - speed;
    return a + speed;
}


