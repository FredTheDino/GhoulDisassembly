#include "fog.h"

#include "main.h"
#include <stdio.h>
#include <vector>
#include <math.h>

ShapeID rect;
Vec2 vec_form_angle(f32 angle) {
    return fog_V2(cos(angle), sin(angle));
}

struct Bullet {
    f32 lifetime;
    Body body;

    static Bullet create(Vec2 position, f32 direction, f32 speed) {
        Body body = fog_physics_create_body(rect, 0.1, 0.1, 0.01);
        body.velocity = vec_form_angle(direction) * speed;
        body.position = position;
        body.scale = fog_V2(0.03, 0.03);
        return { 1, body };
    }

    bool alive() {
        return lifetime > 0;
    }

    void update(f32 delta) {
        fog_physics_integrate(&body, delta);
        lifetime -= delta;
    }

    void draw() {
        fog_renderer_push_point(0, body.position, fog_V4(1, 1, 1, 1), body.scale.x);
        fog_physics_debug_draw_body(&body);
    }
};

struct Badie {
    Body body;
    bool alive;

    static Badie create(Vec2 position) {
        Body body = fog_physics_create_body(rect, 0.0, 0.0, 0.0);
        body.position = position;
        return { body, true };
    }

    void update(f32 delta, std::vector<Bullet> *bullets) {
        for (Bullet &bullet : *bullets) {
            if (fog_physics_check_overlap(&body, &bullet.body).is_valid) {
                alive = false;
            }
        }
    }

    void draw() {
        if (alive) {
            fog_renderer_push_point(0, body.position, fog_V4(1, 1, 1, 1), 0.1);
            fog_physics_debug_draw_body(&body);
        }
    }
};

std::vector<Bullet> bullets;
std::vector<Badie> baddies;

f32 rotate_towards(f32 speed, f32 a, f32 b) {
    // Shortest translation
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

struct Slayer {
    Vec2 position;
    f32 facing;

    f32 speed;
    f32 rotation_speed;
    f32 bullet_speed;
    int ammo;

    void fire() {
        bullets.push_back(Bullet::create(position, facing, bullet_speed));
        ammo--;
    }

    void update(f32 delta) {
        static b8 active = true;
        if (fog_util_begin_tweak_section("Player", &active)) {
            fog_util_show_vec2("pos", position);
            fog_util_tweak_f32("speed", &speed, 0.1);
            fog_util_tweak_f32("rot_speed", &rotation_speed, 0.1);
        }
        fog_util_end_tweak_section(&active);

        Vec2 velocity = fog_V2(fog_input_value(NAME(XINPUT), P1), fog_input_value(NAME(YINPUT), P1));
        f32 movement = fog_length_v2(velocity);
        if (movement) {
            facing = rotate_towards(rotation_speed * delta, facing, fog_angle_v2(velocity));
            position += vec_form_angle(facing) * movement * speed * delta;
        }

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

    for (int i = bullets.size() - 1; i >= 0; i--) {
        Bullet &bullet = bullets[i];
        bullet.update(delta);
        if (!bullet.alive()) {
            bullets.erase(bullets.begin() + i);
        }
    }

    for (Badie &badi: baddies) {
        badi.update(delta, &bullets);
    }
}

void draw() {
    fog_renderer_push_point(1, player.position, fog_V4(1, 0, 0, 1), 0.1);

    for (Bullet &bullet : bullets) {
        bullet.draw();
    }

    for (Badie &badi: baddies) {
        badi.draw();
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

    Vec2 points[] = {
        fog_V2(0, 0),
        fog_V2(1, 0),
        fog_V2(1, 1),
        fog_V2(0, 1),
    };
    rect = fog_physics_add_shape(4, points);

    baddies.push_back(Badie::create(fog_V2(1, 0)));

    player = {};
    player.speed = 0.8;
    player.rotation_speed = 10;
    player.bullet_speed = 5;

    fog_run(update, draw);
    return 0;
}
