#include "lib/neslib.h"
#include "lib/boilerplate.h"
#include "bin/build_info.h"
#include "src/title.h"
#include "graphics/palettes.h"

// Suggestion: Define smart names for your banks and use defines like this. 
// This is just to make a clear example, and I didn't want to suggest using bank #s directly.
#define BANK_TITLE 0

// Ditto, same advice here.
#define CHR_BANK_0 0
#define CHR_BANK_1 2 // NOTE: We have two copies of the same 4k data in the 8k .chr files (because I'm lazy, ok?) so we use bank 2 to get the inverted one.

#define DUMMY_SONG 0
#define SFX_BOING 0 

// Globals! Defined as externs in src/globals.h
unsigned char currentPadState;
unsigned char i;
char currentMessage[16];

// Local to this file.
static unsigned char showMessageA;
static unsigned char playMusic;
static unsigned char chrBank;
static unsigned char mirrorMode;
static char screenBuffer[20];

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
	playMusic = 0;
	mirrorMode = MIRROR_HORIZONTAL;

	// Queue up our dummy song and start playing it.
	music_play(DUMMY_SONG);
	music_pause(playMusic);

	pal_col(1,0x19);//set dark green color
	pal_col(17,0x19);

	set_prg_bank(BANK_TITLE);
	show_title();

	// TODO: Fade anim goes here.
	

	// Now we wait for input from the user, and do dumb things!
	while(1) {
		currentPadState = pad_trigger(0);
		ppu_wait_nmi();
	}
}
