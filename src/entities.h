#pragma once
#include "fog.h"
#include <vector>

struct Bullet {
    f32 lifetime;
    Body body;

    static Bullet create(Vec2 position, f32 direction, f32 speed);

    bool alive() { return lifetime > 0; }

    void update(f32 delta);

    void draw();
};

struct Badie {
    Body body;
    bool alive;

    static Badie create(Vec2 position);

    void update(f32 delta, std::vector<Bullet> *bullets);

    void draw();
};

struct Slayer {
    Vec2 position;
    f32 facing;

    f32 speed;
    f32 rotation_speed;
    f32 bullet_speed;
    int ammo;

    void fire(std::vector<Bullet> *bullets);

    void update(f32 delta, std::vector<Bullet> *bullets);

    void draw();
};

