#pragma once
#include "fog.h"

enum SpriteName {
    PLAYER_STAND,
    PLAYER_WALK,
    PLAYER_RIFLE,
    PLAYER_GUN,

    JUMPER_STAND,
    JUMPER_JUMP,

    SKELL_STAND,
    SKELL_WALK,

    BONE1,
    BONE2,

    ROCK,
    SMALL_ROCKS1,
    SMALL_ROCKS2,
    SMALL_ROCKS3,
    SMALL_ROCKS4,
    TOMB1,
    TOMB2,
    WALL1,
    WALL2,
};

void load_sprite();

void draw_sprite(SpriteName name, Vec2 p, Vec2 scale, f32 rot=0.0);
