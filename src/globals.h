#define PLAYER_DIRECTION_LEFT 0x24
#define PLAYER_DIRECTION_RIGHT 0x20
#define PLAYER_DIRECTION_UP 4
#define PLAYER_DIRECTION_DOWN 0

#define PLAYER_SPRITE_ID 0x10
#define PLAYER_SPRITE_TILE 0
#define PLAYER_VELOCITY 1
#define FIRST_ENEMY_SPRITE_ID 0x20

#define PLAYER_WIDTH 15
#define PLAYER_HEIGHT 15

#define SCREEN_EDGE_LEFT 10
#define SCREEN_EDGE_RIGHT 245
#define SCREEN_EDGE_TOP 10
#define SCREEN_EDGE_BOTTOM 172

#define LEVEL_WIDTH 8
#define LEVEL_HEIGHT 8
#define MAP_TILE_SIZE 192

#define GAME_STATE_RUNNING 0
#define GAME_STATE_REDRAW 1
#define GAME_STATE_WORLD_MOVEMENT 2

#define LEVEL_FRAGMENT_1 0x01
#define LEVEL_FRAGMENT_2 0x02
#define LEVEL_FRAGMENT_1_TILES 24
#define LEVEL_FRAGMENT_2_TILES 32
#define LEVEL_FRAGMENT_TILE_LEN 8


// This file defines globals that can be used all over. You'll want common things in here, as the NES has very, very
// limited ram. 

extern unsigned char currentPadState;
extern unsigned char i, j, scratch, scratch2, scratch3, scratch4, scratch5; 
extern unsigned int scratchInt;
extern unsigned char playerOverworldPosition;
extern unsigned char currentSpriteId;
extern unsigned char gameState;
extern unsigned char playerX, playerY, playerDirection, playerAnimState, playerXVelocity, playerYVelocity;

#pragma zpsym ("currentPadState")
#pragma zpsym ("i")
#pragma zpsym ("j")
#pragma zpsym ("scratchInt")
#pragma zpsym ("playerOverworldPosition")
#pragma zpsym ("currentSpriteId")
#pragma zpsym ("scratch")
#pragma zpsym ("scratch2")
#pragma zpsym ("scratch3")
#pragma zpsym ("scratch4")
#pragma zpsym ("scratch5")
#pragma zpsym ("gameState")
#pragma zpsym ("playerX")
#pragma zpsym ("playerY")
#pragma zpsym ("playerDirection")
#pragma zpsym ("playerAnimState")
#pragma zpsym ("playerXVelocity")
#pragma zpsym ("playerYVelocity")

extern unsigned char currentLevel[256];
extern char screenBuffer[0x30];
extern char currentWorldData[64];
extern char extendedSpriteData[56];




extern void put_str(unsigned int adr, const char *str);
extern void clear_screen();
extern unsigned char test_collision(unsigned char tileId);