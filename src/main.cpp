#include "main.h"
#include <functional>

// Next step:
// - Walls
// - Particles? :o
// - Better Camera
// - Better Controls?


void GameState::init() {
    fog_renderer_set_window_size(WIN_WIDTH, WIN_HEIGHT);
    fog_renderer_turn_on_camera(0);

    fog_input_add_mod(fog_key_to_input_code(SDLK_w), NAME(YINPUT), P1, 1.0f);
    fog_input_add_mod(fog_key_to_input_code(SDLK_s), NAME(YINPUT), P1, -1.0f);
    fog_input_add_mod(fog_key_to_input_code(SDLK_a), NAME(XINPUT), P1, -1.0f);
    fog_input_add_mod(fog_key_to_input_code(SDLK_d), NAME(XINPUT), P1, 1.0f);
    fog_input_add(fog_key_to_input_code(SDLK_SPACE), NAME(SHOOT), P1);
    fog_input_add(fog_key_to_input_code(SDLK_r), NAME(RELOAD), P1);

    Vec2 points[] = {
        fog_V2(0, 0),
        fog_V2(1, 0),
        fog_V2(1, 1),
        fog_V2(0, 1),
    };
    rect = fog_physics_add_shape(4, points);


    player = Slayer::create(fog_V2(0, 0));

    spawn_ghoul();
    spawn_ghoul();
    spawn_ghoul();

    walls.push_back(create_wall(fog_V2(-2, -2)));
    walls.push_back(create_wall(fog_V2( 2, -2)));
    walls.push_back(create_wall(fog_V2( 2,  2)));
    walls.push_back(create_wall(fog_V2(-2,  2)));
}

template <typename T, typename F>
void call_and_filter(std::vector<T> &list, F func) {
    for (int i = list.size() - 1; i >= 0; i--) {
        T &t = list[i];
        func(t);
        if (!t.alive()) {
            list.erase(list.begin() + i);
        }
    }
}

void GameState::spawn_ghoul() {
    baddies.push_back(Badie::create(player.body.position + fog_random_unit_vec2() * 1));
    next_ghoul = fog_logic_now() + fog_random_real(0.5, 3.5);
}

void GameState::update() {
    f32 delta = fog_logic_delta();

    if (fog_logic_now() > next_ghoul) { spawn_ghoul(); }

    player.update(delta, *this);

    call_and_filter(bullets, [delta, this](Bullet &b) { b.update(delta, *this); });
    call_and_filter(baddies, [delta, this](Badie &b) { b.update(delta, *this); });

    fog_renderer_fetch_camera(0)->position = player.body.position;
}

void GameState::draw() {
    player.draw();

    for (Bullet &bullet : bullets) {
        bullet.draw();
    }

    for (Badie &badi: baddies) {
        badi.draw();
    }

    for (Body &wall: walls) {
        fog_physics_debug_draw_body(&wall);
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
