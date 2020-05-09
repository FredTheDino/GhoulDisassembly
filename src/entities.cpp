#include "entities.h"
#include "main.h"
#include "util.h"

Bullet Bullet::create(Vec2 position, f32 direction, f32 accuracy, f32 speed, b8 friendly) {
    Body body = fog_physics_create_body(rect, 0.1, 0.1, 0.9);
    body.velocity = vec_form_angle(direction + fog_random_real(-accuracy, accuracy)) * speed;
    body.position = position;
    body.scale = fog_V2(0.03, 0.03);
    return { 1, body, friendly };
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
    Body body = fog_physics_create_body(rect, 1.0, 0.0, 0.98);
    body.position = position;
    body.scale = fog_V2(0.1, 0.1);
    Badie badie = {body};
    badie.hp = 1;
    badie.step_time = fog_random_real(0.8, 1.3);
    return badie;
}

void Badie::update(f32 delta, std::vector<Bullet> *bullets, Slayer *slayer) {
    if (fog_logic_now() > step) {
        step = fog_logic_now() + step_time;
        f32 angle = fog_angle_v2(slayer->body.position - body.position);
        body.velocity += vec_form_angle(angle + fog_random_real(-0.6, 0.6)) * fog_random_real(1.1, 1.5);
    }
    fog_physics_integrate(&body, delta);
    for (Bullet &bullet : *bullets) {
        if (!bullet.friendly) continue;
        if (fog_physics_check_overlap(&body, &bullet.body).is_valid) {
            bullet.kill();
            kill();
        }
    }
}

void Badie::draw() {
    fog_renderer_push_point(0, body.position, fog_V4(1, 1, 1, 1), body.scale.x);
    fog_physics_debug_draw_body(&body);
}

Slayer Slayer::create(Vec2 position) {
    Slayer slayer = {};
    slayer.body = fog_physics_create_body(rect, 1, 0, 0.94);
    slayer.body.scale = fog_V2(0.1, 0.1);
    slayer.max_ammo = 10;
    slayer.ammo = slayer.max_ammo;
    slayer.hp = 1;

    slayer.acceleration = 40;
    slayer.rotation_speed = 10;
    slayer.bullet_speed = 10;
    return slayer;
}

void Slayer::fire(std::vector<Bullet> *bullets) {
    bullets->push_back(Bullet::create(body.position, body.rotation, 0.02, bullet_speed));
    ammo--;
}

void Slayer::update(f32 delta, std::vector<Bullet> *bullets, std::vector<Badie> *baddies) {
    if (!alive()) return;

    static b8 active = true;
    if (fog_util_begin_tweak_section("Player", &active)) {
        fog_util_show_vec2("pos", body.position);
        fog_util_show_vec2("vel", body.velocity);
        fog_util_show_vec2("acc", body.acceleration);
        fog_util_tweak_f32("acc_force", &acceleration, 0.1);
        fog_util_tweak_f32("rot_speed", &rotation_speed, 0.2);
    }
    fog_util_end_tweak_section(&active);

    for (Badie &badie : *baddies) {
        if (fog_physics_check_overlap(&badie.body, &body).is_valid) {
            kill();
        }
    }

    Vec2 target = fog_input_world_mouse_position(0) - body.position;
    body.rotation = rotate_towards(rotation_speed * delta, body.rotation, fog_angle_v2(target));

    Vec2 direction = fog_V2(fog_input_value(NAME(XINPUT), P1), fog_input_value(NAME(YINPUT), P1));
    body.acceleration = fog_V2(0, 0);
    if (fog_length_v2(direction) > 0.1) {
        body.acceleration = direction * acceleration * delta;
    }
    fog_physics_integrate(&body, delta);


    if (ammo && fog_input_pressed(NAME(SHOOT), P1)) {
        fire(bullets);
    }

    if (fog_input_pressed(NAME(RELOAD), P1)) {
        ammo = max_ammo;
    }
}

void Slayer::draw() {
    if (!alive()) return;
    fog_renderer_push_point(1, body.position, fog_V4(1, 0, 0, 1), 0.1);
    Vec2 forward = vec_form_angle(body.rotation);
    fog_renderer_push_line(1, body.position, body.position + forward, fog_V4(0, 0, 0, 1), 0.01);
    fog_physics_debug_draw_body(&body);
}
