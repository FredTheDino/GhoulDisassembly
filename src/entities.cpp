#include "entities.h"
#include "main.h"
#include "util.h"
#include "sprites.h"

Wall Wall::create(Vec2 position, SpriteName sprite) {
    Body body = fog_physics_create_body(rect, 0, 0, 0);
    body.position = position;
    body.scale = fog_V2(0.1, 0.1);
    return { body, sprite};
}

void Wall::draw() {
    draw_sprite(sprite, body.position, body.scale);
}

Bullet Bullet::create(Vec2 position, f32 direction, f32 accuracy, f32 speed, b8 friendly) {
    Body body = fog_physics_create_body(rect, 0.1, 0.1, 0.9);
    body.velocity = vec_form_angle(direction + fog_random_real(-accuracy, accuracy)) * speed;
    body.position = position;
    body.scale = fog_V2(0.03, 0.03);
    return { 1, body, friendly };
}

void Bullet::update(f32 delta, GameState &gs) {
    fog_physics_integrate(&body, delta);
    lifetime -= delta;

    for (Wall &wall : gs.walls) {
        auto overlap = fog_physics_check_overlap(&wall.body, &body);
        if (overlap.is_valid) { kill(); }
    }
}

void Bullet::draw() {
    fog_renderer_push_point(0, body.position, fog_V4(0.784, 0.141, 0.141, 1), body.scale.x);
    // fog_physics_debug_draw_body(&body);
}

Badie Badie::create(Vec2 position, int type) {
    Body body = fog_physics_create_body(rect, 1.0, 0.0, 0.98);
    body.position = position;
    body.scale = fog_V2(0.1, 0.1);
    Badie badie = {body};

    badie.type = type;
    if (type == 0) {
        badie.hp = 2;
        badie.step_time = fog_random_real(0.8, 1.3);
        badie.body.damping = 0.95;
    } else if (type == 1) {
        badie.hp = 1;
        badie.speed = fog_random_real(2.3, 2.9);
        badie.body.damping = 0.98;
        badie.body.scale *= 0.5;
    } else if (type == 2) {
        badie.hp = 4;
        badie.step_time = fog_random_real(0.4, 0.5);
        badie.body.damping = 0.999;
    }
    return badie;
}

void Badie::update(f32 delta, GameState &gs) {
    Vec2 player_direction = gs.player.body.position - body.position;
    if (type == 0 || type == 2) {
        f32 jump_length;
        if (type == 0) {
            jump_length = fog_random_real(1.1, 1.5);
        } else {
            jump_length = fog_random_real(0.5, 0.);
        }
        if (fog_logic_now() > step) {
            step = fog_logic_now() + step_time * fog_random_real(0.9, 1.1);
            f32 angle = fog_angle_v2(player_direction) + fog_random_real(-0.6, 0.6);
            Vec2 jump_impulse = vec_form_angle(angle) * jump_length;
            body.velocity += jump_impulse;
        }

        for (Wall &wall : gs.walls) {
            auto overlap = fog_physics_check_overlap(&wall.body, &body);
            if (overlap.is_valid) { fog_physics_solve(overlap); }
        }
    }

    if (type == 1) {
        f32 distance = fog_length_squared_v2(player_direction);
        if (distance > 0.3 * 0.3) {
            direction = fog_normalize_v2(player_direction);
        }
        if (fog_length_squared_v2(body.velocity) < speed) {
            body.force = direction * delta;
        } else {
            body.force = {};
        }
    }

    fog_physics_integrate(&body, delta);
    for (Bullet &bullet : gs.bullets) {
        if (!bullet.friendly) continue;
        if (!alive()) break;
        if (fog_physics_check_overlap(&body, &bullet.body).is_valid) {
            bullet.kill();
            body.velocity += bullet.body.velocity * 0.1;
            hp -= 1;
        }
    }
}

void Badie::draw() {
    Vec2 sprite_scale = fog_V2(0.1, 0.1);
    SpriteName sprite;
    if (type == 0) {
        if (fog_logic_now() > (step - 0.3))
            sprite = SpriteName::JUMPER_STAND;
        else
            sprite = SpriteName::JUMPER_JUMP;
    } else if (type == 1) {
        int frame = int((fog_logic_now() / 0.1)) % 2;
        sprite = SpriteName(int(SpriteName::BONE1) + frame);
    } else if (type == 2) {
        if (fog_logic_now() > (step - 0.1))
            sprite = SpriteName::SKELL_STAND;
        else
            sprite = SpriteName::SKELL_WALK;
    }

    draw_sprite(sprite, body.position, sprite_scale);
    // fog_physics_debug_draw_body(&body);
}

Slayer Slayer::create(Vec2 position) {
    Slayer slayer = {};
    slayer.body = fog_physics_create_body(rect, 1, 0, 0.98);
    slayer.body.scale = fog_V2(0.1, 0.1);
    slayer.max_ammo = 2;
    slayer.ammo = slayer.max_ammo;
    slayer.hp = 1;

    slayer.acceleration = 80;
    slayer.rotation_speed = 10;
    slayer.bullet_speed = 10;
    slayer.reload_time = 0.5;
    return slayer;
}

void Slayer::fire(std::vector<Bullet> *bullets) {
    for (u32 i = 0; i < 4; i++) {
        bullets->push_back(Bullet::create(body.position, body.rotation, 0.3, bullet_speed * fog_random_real(0.8, 1.2)));
    }
    ammo--;
    body.velocity -= vec_form_angle(body.rotation) * fog_random_real(1.0, 2.0);
}

void Slayer::update(f32 delta, GameState &gs) {
    if (!alive()) return;
    if (reloading_done > fog_logic_now()) return;

    static b8 active = true;
    if (fog_util_begin_tweak_section("Player", &active)) {
        fog_util_tweak_vec2("pos", &body.position, 1.0);
        fog_util_tweak_vec2("vel", &body.velocity, 0.1);
        fog_util_tweak_vec2("acc", &body.acceleration, 0.1);
        fog_util_tweak_f32("acc_force", &acceleration, 0.1);
        fog_util_tweak_f32("rot_speed", &rotation_speed, 0.2);
    }
    fog_util_end_tweak_section(&active);

    for (Badie &badie : gs.baddies) {
        if (fog_physics_check_overlap(&badie.body, &body).is_valid) {
            kill();
        }
    }

    for (Wall &wall : gs.walls) {
        auto overlap = fog_physics_check_overlap(&wall.body, &body);
        if (overlap.is_valid) {
            fog_physics_solve(overlap);
        }
    }

    b8 r1 = fog_input_down(NAME(RELOAD1), P1);
    b8 r2 = fog_input_down(NAME(RELOAD2), P1);
    if (ammo < max_ammo && r1 && r2) {
        reloading_done = fog_logic_now() + reload_time;
        ammo++;
    }

    if (r1 || r2)
        return;

    Vec2 target;
    if (fog_input_using_controller())
        target = fog_V2(fog_input_value(NAME(AIMX), P1), fog_input_value(NAME(AIMY), P1));
    else
        target = fog_input_world_mouse_position(0) - body.position;

    body.rotation = rotate_towards(rotation_speed * delta, body.rotation, fog_angle_v2(target));

    Vec2 direction = fog_V2(fog_input_value(NAME(XINPUT), P1), fog_input_value(NAME(YINPUT), P1));
    body.acceleration = fog_V2(0, 0);
    moving = fog_length_v2(direction) > 0.1;
    if (moving) { body.acceleration = direction * acceleration * delta; }
    fog_physics_integrate(&body, delta);

    if (ammo && (fog_input_pressed(NAME(SHOOT), P1) || fog_input_mouse_pressed(0))) {
        fire(&gs.bullets);
    }

}

void Slayer::draw() {
    if (!alive()) return;

    // fog_renderer_push_line(1, body.position, body.position + forward * 0.1, fog_V4(0, 0, 0, 1), 0.01);
    Vec2 offset = vec_form_angle(body.rotation) * 0.05;
    Vec2 rifle_scale = body.scale;
    if (offset.x < 0)
        rifle_scale.y *= -1;
    if (reloading_done < fog_logic_now())
        draw_sprite(SpriteName::PLAYER_RIFLE, offset + body.position, rifle_scale, body.rotation);
    if (moving)
        draw_sprite(SpriteName::PLAYER_WALK, body.position, body.scale);
    else
        draw_sprite(SpriteName::PLAYER_STAND, body.position, body.scale);

    Vec2 p = fog_V2(20, 20);
    Vec2 delta_y = fog_V2(0, 40);
    for (s32 i = 0; i < ammo; i++) {
        Vec2 top_left = fog_input_screen_to_world(p + delta_y * i, 0);
        draw_sprite(SpriteName::PLAYER_AMMO, top_left, fog_V2(0.1, 0.1));
    }
}
