#include "main.h"


void GameState::init() {
    fog_renderer_set_window_size(WIN_WIDTH, WIN_HEIGHT);
    fog_renderer_turn_on_camera(0);

    fog_input_add_mod(fog_key_to_input_code(SDLK_w), NAME(YINPUT), P1, 1.0f);
    fog_input_add_mod(fog_key_to_input_code(SDLK_s), NAME(YINPUT), P1, -1.0f);
    fog_input_add_mod(fog_key_to_input_code(SDLK_a), NAME(XINPUT), P1, -1.0f); fog_input_add_mod(fog_key_to_input_code(SDLK_d), NAME(XINPUT), P1, 1.0f); fog_input_add(fog_key_to_input_code(SDLK_e), NAME(SHOOT_RIGHT), P1);
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
}

void GameState::update() {
    f32 delta = fog_logic_delta();


    fog_renderer_fetch_camera(0)->position = player.position;

    player.update(delta, &bullets);

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

void GameState::draw() {
    player.draw();

    for (Bullet &bullet : bullets) {
        bullet.draw();
    }

    for (Badie &badi: baddies) {
        badi.draw();
    }
}

Name bindings[NUM_BINDINGS];
ShapeID rect;

GameState game;
void update_wrp() {
    game.update();
}

void draw_wrp() {
    game.draw();
}

int main(int argc, char **argv) {
    for (u32 i = 0; i < NUM_BINDINGS; i++) {
        bindings[i] = fog_input_request_name(1);
    }

    fog_init(argc, argv);

    game.init();
    fog_run(update_wrp, draw_wrp);
    return 0;
}
