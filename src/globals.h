#define PLAYER_DIRECTION_LEFT 0
#define PLAYER_DIRECTION_RIGHT 1
#define PLAYER_DIRECTION_UP 2
#define PLAYER_DIRECTION_DOWN 3

#define PLAYER_SPRITE_ID 0x10
#define PLAYER_SPRITE_TILE 0
#define PLAYER_VELOCITY 1

#define SCREEN_EDGE_LEFT 10
#define SCREEN_EDGE_RIGHT 245
#define SCREEN_EDGE_TOP 10
#define SCREEN_EDGE_BOTTOM 172

#define LEVEL_WIDTH 4
#define LEVEL_HEIGHT 4

#define GAME_STATE_RUNNING 0
#define GAME_STATE_REDRAW 1


// This file defines globals that can be used all over. You'll want common things in here, as the NES has very, very
// limited ram. 
extern unsigned char currentPadState;
extern unsigned char i, j, scratch, scratch2; 
extern unsigned int scratchInt;
extern unsigned char currentLevel[256];
extern char screenBuffer[0x30];
extern unsigned char playerOverworldPosition;
extern unsigned char currentSpriteId;
extern unsigned char gameState;


extern unsigned char playerX, playerY, playerDirection, playerAnimState, playerXVelocity, playerYVelocity;


extern void put_str(unsigned int adr, const char *str);
extern void clear_screen();