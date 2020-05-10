#pragma once

#include "fog.h"
#include "entities.h"
#include "sprites.h"

#include <stdio.h>
#include <math.h>

#include <vector>

typedef enum {
    XINPUT,
    YINPUT,
    AIMX,
    AIMY,
    SHOOT,
    RELOAD1,
    RELOAD2,
    NUM_BINDINGS
} Binding;
extern Name bindings[NUM_BINDINGS];

#define NAME(binding) bindings[binding]

#define WIN_WIDTH 800
#define WIN_HEIGHT 800

extern ShapeID rect;

struct Decoration {
    SpriteName sprite;
    Vec2 p;

    static Decoration create(Vec2 p);

    void draw();
};

void spawn_bullet_trail(Vec2 p);
void spawn_bullet_hit(Vec2 p);
void spawn_smoke_puff(Vec2 p);
void spawn_death(Vec2 p);

struct GameState {
    std::vector<Bullet> bullets;
    std::vector<Badie> baddies;
    std::vector<Wall> walls;
    std::vector<Decoration> decos;
    Slayer player;
    f32 next_ghoul;
    f32 game_start;

    ParticleSystem bullet_particles;
    ParticleSystem smoke_particles;

    void spawn_ghoul();

    void init();
    void start_game();
    void update();
    void draw();
};
