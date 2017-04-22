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
#define BANK_LEVEL_1 2

#define DUMMY_SONG 0
#define SFX_BOING 0 

// Globals! Defined as externs in src/globals.h
#pragma bssseg (push,"ZEROPAGE")
#pragma dataseg(push,"ZEROPAGE")
unsigned char currentPadState, staticPadState;
unsigned char i;
unsigned char j;
unsigned char playerX, playerY, playerDirection, playerAnimState, playerXVelocity, playerYVelocity, playerVelocityLockTime;
unsigned char playerOverworldPosition;
unsigned char currentSpriteId;
unsigned char gameState;
unsigned char scratch, scratch2, scratch3, scratch4, scratch5;
unsigned char playerHealth, worldChunkCount;
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

	gameState = GAME_STATE_INIT;

	// Now we wait for input from the user, and do dumb things!
	while(1) {
		if (gameState == GAME_STATE_INIT) {

			set_prg_bank(BANK_TITLE);
			show_title();
			bank_spr(1);
			pal_bg(main_palette);
			pal_spr(sprite_palette);
			set_chr_bank_0(CHR_BANK_MAIN);
			set_chr_bank_1(CHR_BANK_MAIN+1);

			// TODO: Should be defined by the level...
			playerOverworldPosition = 27;
			playerX = 80;
			playerY = 80;

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
			ppu_wait_nmi();
		} else if (gameState == GAME_STATE_REDRAW) {
			draw_level();
			gameState = GAME_STATE_RUNNING;
		} else if (gameState == GAME_STATE_WORLD_MOVEMENT) {
			draw_level();
			draw_sprites();
			draw_hud();
			gameState = GAME_STATE_RUNNING;
		} else if (gameState == GAME_STATE_GAME_OVER) {
			// CRASH. FIXME: Don't do that.
			set_prg_bank(BANK_TITLE);
			show_game_over();
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
	set_prg_bank(BANK_LEVEL_1);
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

unsigned char test_collision(unsigned char tileId) {
	char temp = tileId & 0x3f;
	if (temp >= LEVEL_FRAGMENT_1_TILES && temp < LEVEL_FRAGMENT_1_TILES+LEVEL_FRAGMENT_TILE_LEN) {
		if (currentWorldData[playerOverworldPosition] & LEVEL_FRAGMENT_1)
			temp -= LEVEL_FRAGMENT_TILE_LEN;
	} else if (temp >= LEVEL_FRAGMENT_2_TILES && temp < LEVEL_FRAGMENT_2_TILES+LEVEL_FRAGMENT_TILE_LEN) {
		if (currentWorldData[playerOverworldPosition] & LEVEL_FRAGMENT_2)
			temp -= (LEVEL_FRAGMENT_TILE_LEN*2);
	}

	switch (temp) {
		case 2:
		case 21:
		case 24: 
		case 32:
		case 29:
		case 37:
			return 1;
		default:
			return 0;
	}
}

// TODO: Bank me!
void draw_hud() {
	ppu_off();
	vram_adr(0x23f0);
	for (i = 0; i < 16; i++)
		vram_put(0xff);

	vram_adr(NTADR_A(0, 24));
	vram_put(HUD_L);
	vram_fill(HUD_TOP, 30);
	vram_put(HUD_R);

	vram_adr(NTADR_A(1, 25));
	vram_put(HUD_H);
	vram_put(HUD_CHARACTERS+('e'-'a'));
	vram_put(HUD_CHARACTERS+('a'-'a'));
	vram_put(HUD_CHARACTERS+('l'-'a'));
	vram_put(HUD_CHARACTERS+('t'-'a'));
	vram_put(HUD_CHARACTERS+('h'-'a'));
	vram_put(HUD_COLON);
	vram_put(HUD_BLANK);

	for (i = 0; i < 5; i++) {
		if (playerHealth > i)
			vram_put(HUD_HEART);
		else
			vram_put(HUD_BLANK);
	}
	vram_adr(NTADR_A(1,27));
	vram_put(HUD_W);
	vram_put(HUD_CHARACTERS+('o'-'a'));
	vram_put(HUD_CHARACTERS+('r'-'a'));
	vram_put(HUD_CHARACTERS+('l'-'a'));
	vram_put(HUD_CHARACTERS+('d'-'a'));
	vram_put(HUD_BLANK);
	vram_put(HUD_C);
	vram_put(HUD_CHARACTERS+('h'-'a'));
	vram_put(HUD_CHARACTERS+('u'-'a'));
	vram_put(HUD_CHARACTERS+('n'-'a'));
	vram_put(HUD_CHARACTERS+('k'-'a'));
	vram_put(HUD_CHARACTERS+('s'-'a'));
	vram_put(HUD_COLON);
	vram_put(HUD_BLANK);
	vram_put(HUD_NUMBERS);
	vram_put(HUD_NUMBERS+worldChunkCount);

	ppu_on_all();
}

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