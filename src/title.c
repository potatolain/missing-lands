#include "src/globals.h"
#include "lib/neslib.h"
#include "lib/boilerplate.h"
#include "bin/build_info.h"
#pragma rodataseg ("ROM_00")
#pragma codeseg ("ROM_00")

// TODO: We have a nice way to fade our palette. Can we do that please?
void show_title() {

	set_chr_bank_0(CHR_BANK_TITLE);
	set_chr_bank_1(CHR_BANK_TITLE+1);
	ppu_off();
	clear_screen();
	pal_col(1,0x19);//set dark green color
	pal_col(17,0x19);

	// Show a message to the user.
	put_str(NTADR_A(7,6),  "- Missing Lands -");
	put_str(NTADR_A(8,17),  "- Press Start -");

	// Also show some cool build info because we can.
	put_str(NTADR_A(2,23), "Ludum Dare 38 Entry");
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
	put_str(NTADR_A(7, 6), "- Missing Lands -");
	put_str(NTADR_A(2, 12), "You are a tiny frog trapped");
	put_str(NTADR_A(2, 13), "in a tiny world.");
	put_str(NTADR_A(2, 16), "You remember a time where");
	put_str(NTADR_A(2, 17), "your world was vast.");
	put_str(NTADR_A(2, 20), "The pieces of that world");
	put_str(NTADR_A(2, 21), "are scattered around you.");
	put_str(NTADR_A(2, 22), "Can you restore this world");
	put_str(NTADR_A(2, 23), "to its former glory?");
	ppu_on_all();

	while (1) {
		currentPadState = pad_trigger(0);
		if (currentPadState & PAD_START) {
			break;
		}
		ppu_wait_nmi();
	}
		
	
}

void show_game_over() {
	oam_hide_rest(0);
	ppu_off();
	clear_screen();

	set_chr_bank_0(CHR_BANK_TITLE);
	set_chr_bank_1(CHR_BANK_TITLE+1);
	pal_col(1,0x19);//set dark green color
	pal_col(17,0x19);


	// Show a message to the user.
	put_str(NTADR_A(2,8), "GAME OVER!");
	put_str(NTADR_A(2,12), "You died...");
	put_str(NTADR_A(2,17), "- Press Start -");


	ppu_on_all();

	// Wait for start
	while (1) {
		currentPadState = pad_trigger(0);
		if (currentPadState & PAD_START) {
			break;
		}
		ppu_wait_nmi();
	}		
	
}

void show_level_complete() {
	oam_hide_rest(0);
	ppu_off();
	clear_screen();

	set_chr_bank_0(CHR_BANK_TITLE);
	set_chr_bank_1(CHR_BANK_TITLE+1);
	pal_col(1,0x19);//set dark green color
	pal_col(17,0x19);


	// Show a message to the user.
	put_str(NTADR_A(8,8), "Level Complete!");
	put_str(NTADR_A(8,17), "- Press Start -");


	ppu_on_all();

	// Wait for start
	while (1) {
		currentPadState = pad_trigger(0);
		if (currentPadState & PAD_START) {
			break;
		}
		ppu_wait_nmi();
	}		
	
}

void show_win_screen() {
	oam_hide_rest(0);
	ppu_off();
	clear_screen();

	set_chr_bank_0(CHR_BANK_TITLE);
	set_chr_bank_1(CHR_BANK_TITLE+1);
	pal_col(1,0x19);//set dark green color
	pal_col(17,0x19);


	// Show a message to the user.
	put_str(NTADR_A(8,4), "Congratulations!");
	put_str(NTADR_A(2,10), "You have completed Missing");
	put_str(NTADR_A(2,11), "Worlds!");
	put_str(NTADR_A(2,15), "By your diligence, the pieces");
	put_str(NTADR_A(2,16), "of your tiny world have come");
	put_str(NTADR_A(2,17), "back together.");
	put_str(NTADR_A(2,20), "Your journey is now over.");
	put_str(NTADR_A(5,26), "Thanks for playing!");


	ppu_on_all();

	// Wait for start
	while (1) {
		currentPadState = pad_trigger(0);
		if (currentPadState & PAD_START) {
			break;
		}
		ppu_wait_nmi();
	}		
	
}

void banked_draw_hud() {
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
	vram_put(HUD_SLASH);
	vram_put(HUD_NUMBERS);
	vram_put(HUD_NUMBERS+worldTotalChunks);

	ppu_on_all();

}

void show_pause() {
	ppu_off();
	clear_screen();
	vram_adr(NAMETABLE_A+0x3c0);
	vram_fill(0xff, 0x40);

	set_chr_bank_0(CHR_BANK_TITLE);
	set_chr_bank_1(CHR_BANK_TITLE+1);

	// Show a message to the user.
	put_str(NTADR_A(10,15), "- Paused -");

	// IMPORTANT: Don't show sprites... we don't wanna lose those so we don't touch em.
	ppu_on_bg();

	// Wait for start
	while (1) {
		currentPadState = pad_trigger(0);
		if (currentPadState & PAD_START) {
			break;
		}
		ppu_wait_nmi();
	}		
	
}
