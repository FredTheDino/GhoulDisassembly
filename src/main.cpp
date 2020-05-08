#include "fog.h"

#include "main.h"
#include <stdio.h>
#include <vector>

struct Bullet {
    Vec2 position;
    Vec2 velocity;

    void update(f32 delta) {
        position += velocity * delta;
    }

    void draw() {
        fog_renderer_push_point(0, position, fog_V4(1, 1, 1, 1), 0.1);
    }
};

std::vector<Bullet> bullets;

struct Slayer {
    Vec2 position;
    Vec2 facing;

    f32 speed;
    int ammo;

    void fire() {
        Bullet bullet = {};
        bullet.position = position;
        bullet.velocity = fog_V2(0, 1);
        bullets.push_back(bullet);
        ammo--;
    }

    void update(f32 delta) {
        Vec2 velocity = fog_V2(fog_input_value(NAME(XINPUT), P1), fog_input_value(NAME(YINPUT), P1));
        if (fog_length_squared_v2(velocity))
            position += fog_normalize_v2(velocity) * speed * delta;

        if (ammo) {
            if (fog_input_pressed(NAME(SHOOT_LEFT), P1) && ammo % 2) {
                fire();
            }
            if (fog_input_pressed(NAME(SHOOT_RIGHT), P1) && ammo % 2 == 0) {
                fire();
            }
        }

        if (fog_input_pressed(NAME(RELOAD), P1)) {
            ammo = 3;
        }
    }
};


Slayer player;

void update() {
    f32 delta = fog_logic_delta();

    player.update(delta);

    for (Bullet &bullet : bullets) {
        bullet.update(delta);
    }
}

void draw() {
    fog_renderer_push_point(1, player.position, fog_V4(1, 0, 0, 1), 0.1);

    for (Bullet &bullet : bullets) {
        bullet.draw();
    }
}

int main(int argc, char **argv) {
    for (u32 i = 0; i < NUM_BINDINGS; i++) {
        bindings[i] = fog_input_request_name(1);
    }

    fog_init(argc, argv);
    fog_renderer_set_window_size(WIN_WIDTH, WIN_HEIGHT);
    fog_renderer_turn_on_camera(0);

    fog_input_add_mod(fog_key_to_input_code(SDLK_w), NAME(YINPUT), P1, 1.0f);
    fog_input_add_mod(fog_key_to_input_code(SDLK_s), NAME(YINPUT), P1, -1.0f);
    fog_input_add_mod(fog_key_to_input_code(SDLK_a), NAME(XINPUT), P1, -1.0f);
    fog_input_add_mod(fog_key_to_input_code(SDLK_d), NAME(XINPUT), P1, 1.0f);
    fog_input_add(fog_key_to_input_code(SDLK_e), NAME(SHOOT_RIGHT), P1);
    fog_input_add(fog_key_to_input_code(SDLK_q), NAME(SHOOT_LEFT), P1);
    fog_input_add(fog_key_to_input_code(SDLK_r), NAME(RELOAD), P1);

    player = {};
    player.speed = 2.0;

    fog_run(update, draw);
    return 0;
}
