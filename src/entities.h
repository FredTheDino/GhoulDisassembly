#pragma once
#include "fog.h"
#include "sprites.h"
#include <vector>

struct Bullet;
struct Badie;
struct Slayer;
struct GameState;

struct Bullet {
    f32 lifetime;
    Body body;
    b8 friendly;

    static Bullet create(Vec2 position, f32 direction, f32 accuracy, f32 speed, b8 friendly=true);

    bool alive() { return lifetime > 0; }
    void kill() { lifetime = 0; }

    void update(f32 delta, GameState &gs);

    void draw();
};

struct Badie {
    Body body;
    f32 step_time;
    f32 step;
    int hp;

    static Badie create(Vec2 position);

    bool alive() { return hp > 0; }
    void kill() { hp = 0; }

    void update(f32 delta, GameState &gs);

    void draw();
};

struct Slayer {
    Body body;

    f32 facing;
    f32 acceleration;
    f32 rotation_speed;
    f32 bullet_speed;

    f32 accuracy;
    int ammo;
    int max_ammo;
    int hp;

    int moving;
    f32 reloading_done;
    f32 reload_time;

    static Slayer create(Vec2 position);

    void fire(std::vector<Bullet> *bullets);

    void update(f32 delta, GameState &gs);

    bool alive() { return hp > 0; }
    void kill() { hp = 0; }

    void draw();
};

struct Wall {
    Body body;
    SpriteName sprite;

    static Wall create(Vec2 position, SpriteName sprite=SpriteName::WALL1);
    void draw();
};

