#include "entities.h"
#include "main.h"
#include "util.h"

Bullet Bullet::create(Vec2 position, f32 direction, f32 speed) {
    Body body = fog_physics_create_body(rect, 0.1, 0.1, 0.01);
    body.velocity = vec_form_angle(direction) * speed;
    body.position = position;
    body.scale = fog_V2(0.03, 0.03);
    return { 1, body };
}

void Bullet::update(f32 delta) {
    fog_physics_integrate(&body, delta);
    lifetime -= delta;
}

void Bullet::draw() {
    fog_renderer_push_point(0, body.position, fog_V4(1, 1, 1, 1), body.scale.x);
    fog_physics_debug_draw_body(&body);
}

Badie Badie::create(Vec2 position) {
    Body body = fog_physics_create_body(rect, 0.0, 0.0, 0.0);
    body.position = position;
    body.scale = fog_V2(0.1, 0.1);
    return { body, true };
}

void Badie::update(f32 delta, std::vector<Bullet> *bullets) {
    for (Bullet &bullet : *bullets) {
        if (fog_physics_check_overlap(&body, &bullet.body).is_valid) {
            alive = false;
        }
    }
}

void Badie::draw() {
    if (alive) {
        fog_renderer_push_point(0, body.position, fog_V4(1, 1, 1, 1), body.scale.x);
        fog_physics_debug_draw_body(&body);
    }
}

void Slayer::fire(std::vector<Bullet> *bullets) {
    bullets->push_back(Bullet::create(position, facing, bullet_speed));
    ammo--;
}

void Slayer::update(f32 delta, std::vector<Bullet> *bullets) {
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
            fire(bullets);
        }
        if (fog_input_pressed(NAME(SHOOT_RIGHT), P1) && ammo % 2 == 0) {
            fire(bullets);
        }
    }

    if (fog_input_pressed(NAME(RELOAD), P1)) {
        ammo = 3;
    }
}

void Slayer::draw() {
    fog_renderer_push_point(1, position, fog_V4(1, 0, 0, 1), 0.1);
}
