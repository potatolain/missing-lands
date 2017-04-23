#define SPRITE_TYPE_WORLD_PIECE 0x10
#define SPRITE_TYPE_IGNORED 0x11
#define SPRITE_TYPE_ENEMY 0x12
#define SPRITE_SIZE_MASK 0xc0
#define SPRITE_PALETTE_MASK 0x03
#define SPRITE_ANIM_MASK 0x30
#define SPRITE_SIZE_NORMAL 0
#define SPRITE_ANIM_NONE 0
#define SPRITE_ANIM_DEFAULT 0x10
#define SPRITE_ANIM_FULL 0x20

#define SPRITE_PALETTE_0 0
#define SPRITE_PALETTE_1 1
#define SPRITE_PALETTE_2 2
#define SPRITE_PALETTE_3 3

#define SPRITE_DIRECTION_LEFT 0x24
#define SPRITE_DIRECTION_RIGHT 0x20
#define SPRITE_DIRECTION_UP 4
#define SPRITE_DIRECTION_DOWN 0
#define SPRITE_DIRECTION_UNDEF 0xff

#define ENEMY_VELOCITY_LOCK_TIME 20

#define NESLIB_SPRITE_GONE 0xf0

#define SPRITE_MIN_DIRECTION_TIME 60
// Slows things down a lil bit to sane speeds.. every other frame we move.
#define SPRITE_MOVEMENT_SPEED (FRAME_COUNTER & 0x01)

// Eh, heck with it... all sprites are 16x16
#define SPRITE_WIDTH 16
#define SPRITE_HEIGHT 16

void banked_draw_sprites();
void banked_update_sprites();