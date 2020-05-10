#include "main.h"
#include <functional>
#include "sprites.h"

// Next step:
// - Particles? :o
// - Better Camera

const int ARENA_WIDTH = 5;
const f32 TILE_SIZE = 0.1;

GameState game;

void Decoration::draw() {
    draw_sprite(sprite, p, fog_V2(TILE_SIZE, TILE_SIZE));
}

Decoration Decoration::create(Vec2 p) {
    SpriteName decos[] = {
        SpriteName::SMALL_ROCKS1,
        SpriteName::SMALL_ROCKS2,
        SpriteName::SMALL_ROCKS3,
        SpriteName::SMALL_ROCKS4,
        // SpriteName::TOMB1,
        // SpriteName::TOMB2,
    };
    u32 num = sizeof(decos) / sizeof(SpriteName);
    return {decos[fog_random_int() % num], p};
}

void spawn_bullet_trail(Vec2 p) {
    game.bullet_particles.position = p;
    game.bullet_particles.alive_time = {0.6, 0.7};
    game.bullet_particles.spawn_size = {0.02, 0.01};
    game.bullet_particles.die_size = {0.0, 0.0};
    game.bullet_particles.velocity = {0.0, 0.0};
    fog_renderer_particle_spawn(&game.bullet_particles, 1);
}

void spawn_bullet_hit(Vec2 p) {
    game.bullet_particles.position = p;
    game.bullet_particles.alive_time = {0.4, 0.6};
    game.bullet_particles.spawn_size = {0.01, 0.005};
    game.bullet_particles.die_size = {0.0, 0.0};
    game.bullet_particles.velocity = {0.4, 1.1};
    game.bullet_particles.velocity_dir = {0.0, 2 * M_PI};
    fog_renderer_particle_spawn(&game.bullet_particles, 4);
}

void spawn_death(Vec2 p) {
    game.bullet_particles.position = p;
    game.bullet_particles.keep_alive = true;
    game.bullet_particles.one_size = true;
    game.bullet_particles.one_alpha = true;
    game.bullet_particles.spawn_size = {0.05, 0.02};
    game.bullet_particles.damping = {0.01, 0.001};
    game.bullet_particles.velocity = {0.0, 0.6};
    game.bullet_particles.velocity_dir = {0.0, 2 * M_PI};
    fog_renderer_particle_spawn(&game.bullet_particles, 22);
    game.bullet_particles.keep_alive = false;
    game.bullet_particles.one_size = false;
    game.bullet_particles.one_alpha = false;
}

void spawn_smoke_puff(Vec2 p) {
    game.smoke_particles.position = p;
    game.smoke_particles.alive_time = {0.4, 0.6};
    game.smoke_particles.spawn_size = {0.03, 0.01};
    game.smoke_particles.die_size = {0.0, 0.0};
    game.smoke_particles.velocity = {0.2, 0.5};
    game.smoke_particles.velocity_dir = {0.0, 2 * M_PI};
    fog_renderer_particle_spawn(&game.smoke_particles, 5);
}

#include <time.h>
void GameState::init() {
    fog_random_seed(time(NULL));
    fog_renderer_set_window_size(WIN_WIDTH, WIN_HEIGHT);
    fog_renderer_turn_on_camera(0);

    fog_input_add_mod(fog_key_to_input_code(SDLK_w), NAME(YINPUT), P1, 1.0f);
    fog_input_add_mod(fog_key_to_input_code(SDLK_s), NAME(YINPUT), P1, -1.0f);
    fog_input_add_mod(fog_key_to_input_code(SDLK_a), NAME(XINPUT), P1, -1.0f);
    fog_input_add_mod(fog_key_to_input_code(SDLK_d), NAME(XINPUT), P1, 1.0f);
    fog_input_add(fog_key_to_input_code(SDLK_SPACE), NAME(SHOOT), P1);
    fog_input_add(fog_key_to_input_code(SDLK_x), NAME(RELOAD1), P1);
    fog_input_add(fog_key_to_input_code(SDLK_l), NAME(RELOAD2), P1);

    fog_input_add(fog_axis_to_input_code(SDL_CONTROLLER_AXIS_LEFTY, 0), NAME(YINPUT), P1);
    fog_input_add(fog_axis_to_input_code(SDL_CONTROLLER_AXIS_LEFTX, 0), NAME(XINPUT), P1);
    fog_input_add(fog_axis_to_input_code(SDL_CONTROLLER_AXIS_RIGHTY, 0), NAME(AIMY), P1);
    fog_input_add(fog_axis_to_input_code(SDL_CONTROLLER_AXIS_RIGHTX, 0), NAME(AIMX), P1);
    fog_input_add(fog_button_to_input_code(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, 0), NAME(SHOOT), P1);
    fog_input_add(fog_button_to_input_code(SDL_CONTROLLER_BUTTON_DPAD_UP, 0), NAME(RELOAD1), P1);
    fog_input_add(fog_button_to_input_code(SDL_CONTROLLER_BUTTON_Y, 0), NAME(RELOAD2), P1);

    Vec2 points[] = {
        fog_V2(0, 0),
        fog_V2(1, 0),
        fog_V2(1, 1),
        fog_V2(0, 1),
    };
    rect = fog_physics_add_shape(4, points);

    bullet_particles = fog_renderer_create_particle_system(0, 200, fog_V2(0, 0));
    bullet_particles.rotation = {0, 0};
    bullet_particles.one_color = true;
    bullet_particles.spawn_red = {0.784, 0.784};
    bullet_particles.spawn_green = {0.141, 0.141};
    bullet_particles.spawn_blue = {0.141, 0.141};
    bullet_particles.spawn_alpha = {1, 1};
    bullet_particles.die_alpha = {0, 0};

    smoke_particles = fog_renderer_create_particle_system(0, 200, fog_V2(0, 0));
    smoke_particles.one_color = true;
    smoke_particles.rotation = {0, 0};
    smoke_particles.spawn_red = {0.247, 0.247};
    smoke_particles.spawn_green = {0.278, 0.278};
    smoke_particles.spawn_blue = {0.278, 0.278};
    smoke_particles.spawn_alpha = {1, 1};
    smoke_particles.damping = {0.55, 0.60};
    smoke_particles.die_alpha = {0, 0};

    load_sprite();

    start_game();
}

void GameState::start_game() {
    started = false;
    next_ghoul = 0;

    fog_renderer_particle_clear(&bullet_particles);
    fog_renderer_particle_clear(&smoke_particles);
    walls.clear();
    decos.clear();
    baddies.clear();

    player = Slayer::create(fog_V2(0, 0));

    game_start = fog_logic_now();
    spawn_ghoul();
    spawn_ghoul();
    spawn_ghoul();


    for (f32 i = -ARENA_WIDTH; i <= ARENA_WIDTH; i += TILE_SIZE) {
        walls.push_back(Wall::create(fog_V2(i,  ARENA_WIDTH)));
        walls.push_back(Wall::create(fog_V2(i, -ARENA_WIDTH)));
        walls.push_back(Wall::create(fog_V2( ARENA_WIDTH, i)));
        walls.push_back(Wall::create(fog_V2(-ARENA_WIDTH, i)));
    }

    for (f32 x = -ARENA_WIDTH + TILE_SIZE; x < ARENA_WIDTH; x += TILE_SIZE) {
        for (f32 y = -ARENA_WIDTH + TILE_SIZE; y < ARENA_WIDTH; y += TILE_SIZE) {
            f32 choice = fog_random_real(0, 1);
            Vec2 p = fog_V2(x, y);
            if (choice < 0.7) continue;
            if (choice < 0.87) {
                decos.push_back(Decoration::create(p));
                continue;
            }
            if (choice < 0.9) {
                SpriteName decos[] = {
                    SpriteName::TOMB1,
                    SpriteName::TOMB2,
                    SpriteName::ROCK,
                    SpriteName::WALL1,
                    SpriteName::WALL2,
                };
                u32 num = sizeof(decos) / sizeof(SpriteName);
                walls.push_back(Wall::create(p, decos[fog_random_int() % num]));
            }
        }
    }
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
    if (!player.alive()) return;
    Vec2 p;
    int tries = 0;
    do {
        p = player.body.position + fog_random_unit_vec2() * 1.7;
        tries++;
    } while ((abs(p.x) >= ARENA_WIDTH - TILE_SIZE ||
              abs(p.y) >= ARENA_WIDTH - TILE_SIZE) && tries < 10);
    if (tries == 10) return;

    int max_allowed;
    f32 now = fog_logic_now() - game_start;
    if (now < 20)
        max_allowed = 1;
    else if (now < 40)
        max_allowed = 2;
    else if (now < 60)
        max_allowed = 3;

    baddies.push_back(Badie::create(p, fog_random_int() % max_allowed));
    next_ghoul = fog_logic_now() + fog_random_real(0.5, 3.5);
}

void GameState::update() {
    if (!started) {
        player.update(0, *this);
        fog_renderer_fetch_camera(0)->position = player.body.position;
        if (player.full_ammo()) {
            started = true;
            game_start = fog_logic_now();
        }
        return;
    }

    f32 delta = fog_logic_delta();


    fog_renderer_particle_update(&bullet_particles, delta);
    fog_renderer_particle_update(&smoke_particles, delta);

    if (fog_logic_now() > next_ghoul) { spawn_ghoul(); }

    player.update(delta, *this);

    call_and_filter(bullets, [delta, this](Bullet &b) { b.update(delta, *this); });
    call_and_filter(baddies, [delta, this](Badie &b) { b.update(delta, *this); });

    fog_renderer_fetch_camera(0)->position = player.body.position;
}

void GameState::draw() {
    for (Decoration &deco: decos) { deco.draw(); }

    for (Wall &wall: walls) { wall.draw(); }

    fog_renderer_particle_draw(&bullet_particles);
    fog_renderer_particle_draw(&smoke_particles);

    for (Badie &badi: baddies) { badi.draw(); }

    for (Bullet &bullet : bullets) { bullet.draw(); }

    player.draw();

    Vec2 center = fog_renderer_fetch_camera(0)->position;
    if (!started) {
        fog_renderer_push_sprite_rect(0, center + fog_V2(0.0, 0.6), fog_V2(1.5, 1.5 / 6.0), 0,
                fog_asset_fetch_id("LOGO"),
                fog_V2(0, 512 - 24), fog_V2(144, 24), fog_V4(1, 1, 1, 1));
    }

    if (!started || !player.alive()) {
        Vec2 r1 = center + fog_V2(-0.2, 0.2);
        Vec2 r2 = center + fog_V2( 0.2, 0.2);

        b8 held_r1 = fog_input_down(NAME(RELOAD1), P1);
        b8 held_r2 = fog_input_down(NAME(RELOAD2), P1);

        if (held_r1)
            r1 += fog_random_unit_vec2() * 0.01;
        if (held_r2)
            r2 += fog_random_unit_vec2() * 0.01;

        if (!player.alive() && held_r1 && held_r2) start_game();

        if (fog_input_using_controller()) {
            fog_renderer_push_sprite_rect(0, r1, fog_V2(0.2, 0.2), 0,
                    fog_asset_fetch_id("LOGO"),
                    fog_V2(0, 512 - 40), fog_V2(16, 16), fog_V4(1, 1, 1, 1));

            fog_renderer_push_sprite_rect(0, r2, fog_V2(0.2, 0.2), 0,
                    fog_asset_fetch_id("LOGO"),
                    fog_V2(24, 512 - 40), fog_V2(16, 16), fog_V4(1, 1, 1, 1));
        } else {
            fog_renderer_push_sprite_rect(0, r1, fog_V2(0.2, 0.2), 0,
                    fog_asset_fetch_id("LOGO"),
                    fog_V2(0, 512 - 56), fog_V2(16, 16), fog_V4(1, 1, 1, 1));

            fog_renderer_push_sprite_rect(0, r2, fog_V2(0.2, 0.2), 0,
                    fog_asset_fetch_id("LOGO"),
                    fog_V2(24, 512 - 56), fog_V2(16, 16), fog_V4(1, 1, 1, 1));
        }
    }
}

Name bindings[NUM_BINDINGS];
ShapeID rect;

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
