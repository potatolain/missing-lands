#define DEBUG 0
#define NUMBER_OF_LEVELS 6
#define PLAYER_DIRECTION_LEFT 0x24
#define PLAYER_DIRECTION_RIGHT 0x20
#define PLAYER_DIRECTION_UP 4
#define PLAYER_DIRECTION_DOWN 0
#define PLAYER_SPRITE_EMPTY 0xee

#define PLAYER_SPRITE_ID 0x10
#define PLAYER_SPRITE_TILE 0
#define PLAYER_VELOCITY 1
#define FIRST_ENEMY_SPRITE_ID 0x20

#define PLAYER_WIDTH 15
#define PLAYER_HEIGHT 15

#define SCREEN_EDGE_LEFT 10
#define SCREEN_EDGE_RIGHT 237
#define SCREEN_EDGE_TOP 10
#define SCREEN_EDGE_BOTTOM 172

#define LEVEL_WIDTH 8
#define LEVEL_HEIGHT 8
#define MAP_TILE_SIZE 192

#define GAME_STATE_RUNNING 0
#define GAME_STATE_REDRAW 1
#define GAME_STATE_WORLD_MOVEMENT 2
#define GAME_STATE_LEVEL_COMPLETE 3
#define GAME_STATE_START_LEVEL 4
#define GAME_STATE_PAUSE 100
#define GAME_STATE_INIT 255
#define GAME_STATE_GAME_OVER 250
#define GAME_STATE_WIN 251

#define LEVEL_FRAGMENT_1 0x01
#define LEVEL_FRAGMENT_2 0x02
#define LEVEL_FRAGMENT_1_TILES 24
#define LEVEL_FRAGMENT_2_TILES 32
#define LEVEL_FRAGMENT_TILE_LEN 8

#define HUD_CHARACTERS 0xc1
#define HUD_NUMBERS 0xf6
#define HUD_HEART 0xe0
#define HUD_H 0xe1
#define HUD_W 0xe2
#define HUD_C 0xe3
#define HUD_COLON 0xe4
#define HUD_BLANK 0xf5
#define HUD_TOP 0xe5
#define HUD_L 0xe6
#define HUD_R 0xe7
#define HUD_SLASH 0xdb

#define TILE_EXIT 15
#define TILE_EXIT_CLOSED 14

#define CHR_BANK_TITLE 0
#define CHR_BANK_MAIN 2 // NOTE: We have two copies of the same 4k data in the 8k .chr files (because I'm lazy, ok?) so we use bank 2 to get the inverted one.

#define SFX_HURT 1
#define SFX_PAUSE 2
#define SFX_UNPAUSE 3
#define SFX_GAME_START 4
#define SFX_INTRO_BLIP 5
#define SFX_DEATH 6
#define SFX_LEVEL_COMPLETE 7
#define SFX_EARTH_CHANGE 8
#define SFX_HEART 9

// This file defines globals that can be used all over. You'll want common things in here, as the NES has very, very
// limited ram. 

extern unsigned char currentPadState, staticPadState;
extern unsigned char i, j, scratch, scratch2, scratch3, scratch4, scratch5; 
extern unsigned int scratchInt;
extern unsigned char playerOverworldPosition, currentLevelId;
extern unsigned char currentSpriteId;
extern unsigned char gameState;
extern unsigned char playerX, playerY, playerDirection, playerAnimState, playerXVelocity, playerYVelocity, playerVelocityLockTime, playerInvulnTime;
extern unsigned char playerHealth, worldChunkCount, worldTotalChunks;
extern unsigned char FRAME_COUNTER;

#pragma zpsym ("currentPadState")
#pragma zpsym ("staticPadState")
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
#pragma zpsym ("playerHealth")
#pragma zpsym ("worldChunkCount")
#pragma zpsym ("playerVelocityLockTime")
#pragma zpsym ("playerInvulnTime")
#pragma zpsym ("FRAME_COUNTER")
#pragma zpsym ("worldTotalChunks")
#pragma zpsym ("currentLevelId")

extern unsigned char currentLevel[256];
extern char screenBuffer[0x30];
extern char currentWorldData[64];
extern char extendedSpriteData[56];
extern char world_sprite_state[64];
extern const unsigned char BYTE_TO_BIT[8];

// Hackattack- get at the stuff in level data from whatever bank is loaded.
extern const char lvl_details[4];


extern void put_str(unsigned int adr, const char *str);
extern void clear_screen();
extern unsigned char test_collision(unsigned char tileId, unsigned char isPlayer);
void update_hud();

void animate_fadeout(unsigned char _delay);
void animate_fadein(unsigned char _delay);