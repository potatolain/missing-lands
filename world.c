#include "lib/neslib.h"
#include "lib/boilerplate.h"
#include "bin/build_info.h"
#include "src/title.h"
#include "graphics/palettes.h"
#include "levels/processed/lvl1_tiles.h"

// Suggestion: Define smart names for your banks and use defines like this. 
// This is just to make a clear example, and I didn't want to suggest using bank #s directly.
#define BANK_TITLE 0

// Ditto, same advice here.
#define CHR_BANK_TITLE 0
#define CHR_BANK_MAIN 2 // NOTE: We have two copies of the same 4k data in the 8k .chr files (because I'm lazy, ok?) so we use bank 2 to get the inverted one.

#define DUMMY_SONG 0
#define SFX_BOING 0 
#define MAP_TILE_SIZE = (16*12)

// Globals! Defined as externs in src/globals.h
unsigned char currentPadState;
unsigned char i;
unsigned char j;
unsigned char playerOverworldPosition;
char currentMessage[16];

// Local to this file.
// TODO: Pragma this guy into zp
static unsigned char showMessageA;
static unsigned char playMusic;
static unsigned char scratch, scratch2;
static unsigned int scratchInt;
static unsigned char mirrorMode;
static char screenBuffer[0x30];
static char currentLevel[256];

void draw_level();

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

	showMessageA = 0;
	playMusic = 1;
	mirrorMode = MIRROR_HORIZONTAL;

	// Queue up our dummy song and start playing it.
	music_play(DUMMY_SONG);
	music_pause(playMusic);

	pal_col(1,0x19);//set dark green color
	pal_col(17,0x19);

	set_prg_bank(BANK_TITLE);
	show_title();
	pal_bg(main_palette);
	pal_spr(main_palette);
	set_chr_bank_0(CHR_BANK_MAIN);
	set_chr_bank_1(CHR_BANK_MAIN+1);

	playerOverworldPosition = 0;
	// TODO: Fade anim goes here.
	draw_level();

	// Now we wait for input from the user, and do dumb things!
	while(1) {
		currentPadState = pad_trigger(0);
		if (currentPadState & PAD_RIGHT && playerOverworldPosition < 0x10) {
			playerOverworldPosition += 0x4;
			draw_level();
		} else if (currentPadState & PAD_LEFT && playerOverworldPosition >= 0x04) {
			playerOverworldPosition -= 0x4;
			draw_level();
		} else if (currentPadState & PAD_UP && playerOverworldPosition > 0) {
			playerOverworldPosition -= 1;
			draw_level();
		} else if (currentPadState & PAD_DOWN && playerOverworldPosition < 0x10) {
			playerOverworldPosition += 1;
			draw_level();
		}
		ppu_wait_nmi();
	}
}

void draw_level() {
	ppu_off();
	vram_inc(0);
	vram_adr(NAMETABLE_A);
	// Load up the data into currentLevel
	memcpy(currentLevel, lvl1 + (playerOverworldPosition << 8), 256);

	for (i = 0; i < sizeof(screenBuffer); i++)
		screenBuffer[i] = 0;
	j = -1; 
	// And unpack into vram
	for (i = 0; i != 192; ++i) {
		// TODO: This is like a whirlwind of awful and inefficient. At the very least move this logic to another bank to make me feel better
		scratch = (currentLevel[i] & 0x3f) << 1; // Skip top 2 bytes. That's our palette bytes.
		scratchInt
	 = ((i / 16) << 6) + ((i % 16) << 1);
		vram_adr(NAMETABLE_A+scratchInt
	);
		vram_put(scratch);
		vram_put(scratch+1);
		vram_adr(NAMETABLE_A + scratchInt
	 + 32);
		vram_put(scratch + 16);
		vram_put(scratch + 17);
		// FIXME: Palette bytes
		scratch = (currentLevel[i] & 0xc0); // Top 2 bits are the palette bytes... but where do they need to go?
		if (i % 32 == 16) 
			j -= 8;
		if (i % 2 == 0) 
			j++;
		if (i % 2 == 0) {
			// Even/left
			if ((i / 16) % 2 == 0) {
				// top
				scratch >>= 6;
			} else {
				//bottom
				scratch >>= 2;
			}
		} else {
			// Odd/right
			if ((i / 16) % 2 == 0) {
				// Top
				scratch >>= 4;
			} else {
				// Bottom 
				scratch >>= 0;
			}
		}
		screenBuffer[j] += scratch;
		//scratchInt = NAMETABLE_A+0x3c0+(i>>);
		vram_adr(NAMETABLE_A + scratchInt>>2);
	}
	vram_adr(NAMETABLE_A + 0x3c0);
	vram_write(screenBuffer, 0x30);
	//vram_fill(2, 0x3c0);
	vram_fill(0, 0x30);
	ppu_on_all();
}