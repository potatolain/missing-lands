#include "src/globals.h"
#include "lib/neslib.h"
#include "bin/build_info.h"
#pragma rodataseg ("ROM_00")
#pragma codeseg ("ROM_00")

// TODO: We have a nice way to fade our palette. Can we do that please?
void show_title() {
	// Show a message to the user.
	put_str(NTADR_A(2,8), "SMALL WORLD!");
	put_str(NTADR_A(2,12), "Ludum Dare 38 Game");
	put_str(NTADR_A(2,13), "Press Start!");

	// Also show some cool build info because we can.
	put_str(NTADR_A(2,24), "Built: " BUILD_DATE);
	put_str(NTADR_A(2,25), "Build #" BUILD_NUMBER_STR " (" GIT_COMMIT_ID_SHORT " - " GIT_BRANCH ")");
	put_str(NTADR_A(2,26), "Commit counter: " COMMIT_COUNT_STR);
	ppu_on_all();

	// Wait for start
	while (1) {
		currentPadState = pad_trigger(0);
		if (currentPadState & PAD_START) {
			break;
		}
		ppu_wait_nmi();
	}

	ppu_off();
	clear_screen();
	put_str(NTADR_A(2, 8), "SMALL WORLD!");
	put_str(NTADR_A(2, 12), "You are a tiny frog trapped");
	put_str(NTADR_A(2, 13), "in a small world.");
	put_str(NTADR_A(2, 16), "You remember a time where");
	put_str(NTADR_A(2, 17), "the world was great.");
	put_str(NTADR_A(2, 20), "Pieces of the world are");
	put_str(NTADR_A(2, 21), "scattered around you. Can");
	put_str(NTADR_A(2, 22), "you restore the world to its");
	put_str(NTADR_A(2, 23), "former glory?");
	ppu_on_all();

	while (1) {
		currentPadState = pad_trigger(0);
		if (currentPadState & PAD_START) {
			break;
		}
		ppu_wait_nmi();
	}
		
	
}

