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
    SHOOT,
    RELOAD,
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

struct GameState {
    std::vector<Bullet> bullets;
    std::vector<Badie> baddies;
    std::vector<Body> walls;
    std::vector<Decoration> decos;
    Slayer player;
    f32 next_ghoul;

    void spawn_ghoul();

    void init();
    void update();
    void draw();
};


