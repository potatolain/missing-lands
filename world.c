#include "lib/neslib.h"
#include "lib/boilerplate.h"
#include "bin/build_info.h"
#include "src/title.h"
#include "graphics/palettes.h"
#include "levels/processed/lvl1_tiles.h"
#include "src/level_manip.h"
#include "src/movement.h"
#include "src/globals.h"
#include "src/sprites.h"

// Suggestion: Define smart names for your banks and use defines like this. 
// This is just to make a clear example, and I didn't want to suggest using bank #s directly.
#define BANK_TITLE 0
#define BANK_LEVEL_MANIP 1
#define BANK_MOVEMENT 1
#define BANK_SPRITES 1
// Everything past this bank is levels. Wee.
#define BANK_FIRST_LEVEL 2

#define DUMMY_SONG 0
#define SFX_BOING 0 

// Globals! Defined as externs in src/globals.h
#pragma bssseg (push,"ZEROPAGE")
#pragma dataseg(push,"ZEROPAGE")
unsigned char currentPadState, staticPadState;
unsigned char i;
unsigned char j;
unsigned char playerX, playerY, playerDirection, playerAnimState, playerXVelocity, playerYVelocity, playerVelocityLockTime;
unsigned char playerOverworldPosition, currentLevelId;
unsigned char currentSpriteId;
unsigned char gameState;
unsigned char scratch, scratch2, scratch3, scratch4, scratch5;
unsigned char playerHealth, worldChunkCount, worldTotalChunks;
unsigned int scratchInt;
#pragma bssseg (pop)
#pragma dataseg(pop)


// 4 bytes of data for each sprite available on the level.
char extendedSpriteData[56];
char screenBuffer[0x30];
char currentLevel[256];
char currentWorldData[64];


// Local to this file.
// TODO: Pragma this guy into zp
static unsigned char playMusic;
static unsigned char mirrorMode;

// Foward definitions. No they're not in a header. Go away this is a game jam. Clear enough for 2 days of development.
void draw_level();
void draw_sprites();
void do_movement();
void draw_hud();
void update_sprites();

void clear_screen() {
	// Clear the screen to start
	vram_adr(0x2000);
	vram_fill(0, 0x0400);
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

	mirrorMode = MIRROR_HORIZONTAL;

	gameState = GAME_STATE_INIT;

	// Now we wait for input from the user, and do dumb things!
	while(1) {
		if (gameState == GAME_STATE_INIT) {

			music_play(DUMMY_SONG);
			music_pause(0);


			set_prg_bank(BANK_TITLE);
			show_title();
			set_rand(FRAME_COUNTER);
			// FIXME: Once there's some ingame music, swap this back on with a new song in GAME_STATE_START_LEVEL
			// Then also on/off in pause.
			music_pause(1);

			currentLevelId = 0;
			gameState = GAME_STATE_START_LEVEL;
		} else if (gameState == GAME_STATE_START_LEVEL) {

			bank_spr(1);
			pal_bg(main_palette);
			pal_spr(sprite_palette);
			set_chr_bank_0(CHR_BANK_MAIN);
			set_chr_bank_1(CHR_BANK_MAIN+1);

			set_prg_bank(BANK_FIRST_LEVEL+currentLevelId);

			playerOverworldPosition = *(char*)(lvl_details);
			playerX = *(char*)(lvl_details+1);
			playerY = *(char*)(lvl_details+2);
			worldTotalChunks = *(char*)(lvl_details+3);


			playerHealth = 5;
			worldChunkCount = 0;
			playerDirection = PLAYER_DIRECTION_DOWN;
			playerAnimState = 0;
			playerXVelocity = 0;
			playerYVelocity = 0;
			playerVelocityLockTime = 0;


			for (i = 0; i < 64; i++)
				currentWorldData[i] = 0;

			// TODO: Fade anim goes here.
			draw_level();
			draw_sprites();
			draw_hud();
			gameState = GAME_STATE_RUNNING;

		} else if (gameState == GAME_STATE_RUNNING) {
			staticPadState = pad_trigger(0);
			currentPadState = pad_poll(0);
			do_movement();
			update_sprites();
			ppu_wait_nmi();
		} else if (gameState == GAME_STATE_PAUSE) {

			set_prg_bank(BANK_TITLE);
			show_pause();
			sfx_play(SFX_UNPAUSE, 2);
			gameState = GAME_STATE_REDRAW;

		} else if (gameState == GAME_STATE_REDRAW) {
			set_chr_bank_0(CHR_BANK_MAIN);
			set_chr_bank_1(CHR_BANK_MAIN+1);

			draw_level();
			draw_hud();
			gameState = GAME_STATE_RUNNING;
		} else if (gameState == GAME_STATE_WORLD_MOVEMENT) {
			draw_level();
			draw_sprites();
			draw_hud();
			gameState = GAME_STATE_RUNNING;
		} else if (gameState == GAME_STATE_GAME_OVER) {
			sfx_play(SFX_DEATH, 0);
			set_prg_bank(BANK_TITLE);
			show_game_over();
			gameState = GAME_STATE_INIT;
		} else if (gameState == GAME_STATE_LEVEL_COMPLETE) {
			currentLevelId++;
			if (currentLevelId < NUMBER_OF_LEVELS) {
				sfx_play(SFX_LEVEL_COMPLETE, 1);
				set_prg_bank(BANK_TITLE);
				show_level_complete();
				gameState = GAME_STATE_START_LEVEL;
			} else {
				gameState = GAME_STATE_WIN;
			}
		} else if (gameState == GAME_STATE_WIN) {
			set_prg_bank(BANK_TITLE);
			show_win_screen();
			gameState = GAME_STATE_INIT;
		}
	}
}

void do_movement() {
	set_prg_bank(BANK_MOVEMENT);
	do_banked_movement();
}

void load_screen() {
	// Load up the data into currentLevel
	set_prg_bank(BANK_FIRST_LEVEL+currentLevelId);

	// NOTE: Yes, this says lvl1 - it'll line up with whatever we get though.
	memcpy(currentLevel, lvl1 + (playerOverworldPosition << 8), 256);

}

void draw_level() {
	load_screen();
	set_prg_bank(BANK_LEVEL_MANIP);
	banked_draw_level();
}

void draw_sprites() {
	set_prg_bank(BANK_SPRITES);
	banked_draw_sprites();
}

unsigned char test_collision(unsigned char tileId, unsigned char isPlayer) {
	char temp = tileId & 0x3f;
	if (temp >= LEVEL_FRAGMENT_1_TILES && temp < LEVEL_FRAGMENT_1_TILES+LEVEL_FRAGMENT_TILE_LEN) {
		if (currentWorldData[playerOverworldPosition] & LEVEL_FRAGMENT_1)
			temp -= LEVEL_FRAGMENT_TILE_LEN;
	} else if (temp >= LEVEL_FRAGMENT_2_TILES && temp < LEVEL_FRAGMENT_2_TILES+LEVEL_FRAGMENT_TILE_LEN) {
		if (currentWorldData[playerOverworldPosition] & LEVEL_FRAGMENT_2)
			temp -= (LEVEL_FRAGMENT_TILE_LEN*2);
	}

	switch (temp) {
		case 15: 
			if (isPlayer && worldChunkCount >= worldTotalChunks) {
				gameState = GAME_STATE_LEVEL_COMPLETE;
			}
			// else fallthru
		// This would probably be cleaner with a bunch of if statements
		case 1:
		case 3:
		case 4:
		case 9:
		case 10: 
		case 11:
		case 16: 
		case 17:
		case 18:
		case 19:
		case 20:
		case 22:
		case 23:
		case 25:
		case 26:
		case 27:
		case 28:
		case 33:
		case 34:
		case 35:
		case 36:
			return 0;
		default:
			return 1;
	}
}

void draw_hud() {
	set_prg_bank(BANK_TITLE);
	banked_draw_hud();
}

void update_sprites() {
	set_prg_bank(BANK_SPRITES);
	banked_update_sprites();
}

// TODO: Should I bank this? Not sure how much it helps..
void update_hud() {
	screenBuffer[0] = MSB(NTADR_A(9, 25)) | NT_UPD_HORZ;
	screenBuffer[1] = LSB(NTADR_A(9, 25));
	screenBuffer[2] = 5u;
	for (i = 0; i < 5; ++i) 
		if (playerHealth > i)
			screenBuffer[i+3u] = HUD_HEART;
		else 
			screenBuffer[i+3u] = HUD_BLANK;
	screenBuffer[8] = MSB(NTADR_A(16, 27));
	screenBuffer[9] = LSB(NTADR_A(16, 27));
	screenBuffer[10] = HUD_NUMBERS+worldChunkCount;
	screenBuffer[11] = NT_UPD_EOF;
	set_vram_update(screenBuffer);

}