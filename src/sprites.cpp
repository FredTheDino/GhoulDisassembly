#include "sprites.h"
#include "stdio.h"

const Vec2 SPRITE_DIM = fog_V2(8.0, 8.0);
AssetID sprite_sheet;

Vec2 uv(SpriteName name) {
    int x, y;
    switch (name) {
        case PLAYER_STAND:
        case PLAYER_WALK:
        case PLAYER_RIFLE:
        case PLAYER_GUN:
            x = name - PLAYER_STAND;
            y = 0;
            break;
        case JUMPER_STAND:
        case JUMPER_JUMP:
        case SKELL_STAND:
        case SKELL_WALK:
        case BONE1:
        case BONE2:
            x = name - JUMPER_STAND;
            y = 1;
            break;
        case ROCK:
        case SMALL_ROCKS1:
        case SMALL_ROCKS2:
        case SMALL_ROCKS3:
        case SMALL_ROCKS4:
        case TOMB1:
        case TOMB2:
            x = name - ROCK;
            y = 2;
            break;
        case WALL1:
        case WALL2:
            x = name - WALL1;
            y = 3;
            break;
        default:
            printf("Failed to find spritename\n");
    };
    return {float((x) * 8), 512 - float((y + 1) * 8)};
}

void load_sprite() {
    sprite_sheet = fog_asset_fetch_id("SPRITES");
}

void draw_sprite(SpriteName name, Vec2 p, Vec2 scale, f32 rot) {
    Vec2 uv_min = uv(name);
    fog_renderer_push_sprite_rect(0, p, scale, rot,
            sprite_sheet, uv_min, SPRITE_DIM,
            fog_V4(1, 1, 1, 1));
}

