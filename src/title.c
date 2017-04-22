#include "src/globals.h"
#include "lib/neslib.h"
#include "bin/build_info.h"
#pragma rodataseg ("ROM_00")
#pragma codeseg ("ROM_00")

void show_title() {
		// Show a message to the user.
	put_str(NTADR_A(2,8), "SMALL WORLD!");
	put_str(NTADR_A(2,12), "Ludum Dare 38 Game");
	put_str(NTADR_A(2,13), "Game coming soon...");

	// Also show some cool build info because we can.
	put_str(NTADR_A(2,24), "Built: " BUILD_DATE);
	put_str(NTADR_A(2,25), "Build #" BUILD_NUMBER_STR " (" GIT_COMMIT_ID_SHORT " - " GIT_BRANCH ")");
	put_str(NTADR_A(2,26), "Commit counter: " COMMIT_COUNT_STR);
	ppu_on_all();
}

