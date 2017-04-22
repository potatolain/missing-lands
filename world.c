#include "lib/neslib.h"
#include "lib/boilerplate.h"
#include "bin/build_info.h"
#include "src/title.h"
#include "graphics/palettes.h"
#include "levels/processed/lvl1_tiles.h"
#include "src/level_manip.h"
#include "src/movement.h"
#include "src/globals.h"

// Suggestion: Define smart names for your banks and use defines like this. 
// This is just to make a clear example, and I didn't want to suggest using bank #s directly.
#define BANK_TITLE 0
#define BANK_LEVEL_MANIP 1
#define BANK_MOVEMENT 1
#define BANK_LEVEL_1 2

// Ditto, same advice here.
#define CHR_BANK_TITLE 0
#define CHR_BANK_MAIN 2 // NOTE: We have two copies of the same 4k data in the 8k .chr files (because I'm lazy, ok?) so we use bank 2 to get the inverted one.

#define DUMMY_SONG 0
#define SFX_BOING 0 
#define MAP_TILE_SIZE 192

// Globals! Defined as externs in src/globals.h
unsigned char currentPadState, staticPadState;
unsigned char i;
unsigned char j;
unsigned char playerX, playerY, playerDirection, playerAnimState, playerXVelocity, playerYVelocity;
unsigned char playerOverworldPosition;
unsigned char currentSpriteId;
unsigned char gameState;
char currentMessage[16];
char screenBuffer[0x30];
char currentLevel[256];
unsigned char scratch, scratch2;
unsigned int scratchInt;


// Local to this file.
// TODO: Pragma this guy into zp
static unsigned char playMusic;
static unsigned char mirrorMode;

// Foward definitions. No they're not in a header. Go away this is a game jam. Clear enough for 2 days of development.
void draw_level();
void do_movement();

void clear_screen() {
	// Clear the screen to start
	vram_adr(0x2060);
	vram_fill(0, 0x03a0);
}


// Put a string on the screen at X/Y coordinates given in adr.
void put_str(unsigned int adr, const char *str) {
	vram_adr(adr);
	while(1) {
		if(!*str) break;
		vram_put((*str++)-0x20);//-0x20 because ASCII code 0x20 is placed in tile 0 of the CHR
	}
}

void write_screen_buffer(unsigned char x, unsigned char y, char* data) {
	screenBuffer[0] = MSB(NTADR_A(x, y)) | NT_UPD_HORZ;
	screenBuffer[1] = LSB(NTADR_A(x, y));
	screenBuffer[2] = 16u;
	for (i = 0; data[i] != '\0'; ++i) 
		screenBuffer[i+3u] = data[i]-0x20;
	screenBuffer[19] = NT_UPD_EOF;
	set_vram_update(screenBuffer);
}

// Main entry point for the application.
void main(void) {

	playMusic = 1;
	mirrorMode = MIRROR_HORIZONTAL;

	// Queue up our dummy song and start playing it.
	music_play(DUMMY_SONG);
	music_pause(playMusic);

	pal_col(1,0x19);//set dark green color
	pal_col(17,0x19);

	set_prg_bank(BANK_TITLE);
	show_title();
	bank_spr(1);
	pal_bg(main_palette);
	pal_spr(main_palette);
	set_chr_bank_0(CHR_BANK_MAIN);
	set_chr_bank_1(CHR_BANK_MAIN+1);

	playerOverworldPosition = 0;
	playerX = 50;
	playerY = 50;
	playerDirection = PLAYER_DIRECTION_DOWN;
	// TODO: Fade anim goes here.
	draw_level();

	// Now we wait for input from the user, and do dumb things!
	while(1) {
		if (gameState == GAME_STATE_RUNNING) {
			staticPadState = pad_trigger(0);
			currentPadState = pad_poll(0);
			do_movement();
			ppu_wait_nmi();
		} else if (gameState == GAME_STATE_REDRAW) {
			draw_level();
			gameState = GAME_STATE_RUNNING;
		}
	}
}

void do_movement() {
	set_prg_bank(BANK_MOVEMENT);
	do_banked_movement();
}

void load_screen() {
	// Load up the data into currentLevel
	set_prg_bank(BANK_LEVEL_1);
	memcpy(currentLevel, lvl1 + (playerOverworldPosition << 8), 256);

}

void draw_level() {
	load_screen();
	set_prg_bank(BANK_LEVEL_MANIP);
	banked_draw_level();
}

unsigned char test_collision(unsigned char tileId) {
	switch (tileId & 0x3f) {
		case 2:
			return 1;
		default:
			return 0;
	}
}