#include "src/globals.h"
#include "lib/neslib.h"
#pragma rodataseg ("ROM_01")
#pragma codeseg ("ROM_01")

void do_banked_movement() {
	if (currentPadState & PAD_RIGHT) {
		playerXVelocity = PLAYER_VELOCITY;
	} else if (currentPadState & PAD_LEFT) {
		playerXVelocity = 0u-PLAYER_VELOCITY;
	} else {
		playerXVelocity = 0;
	}

	if (currentPadState & PAD_UP) {
		playerYVelocity = 0u-PLAYER_VELOCITY;
	} else if (currentPadState & PAD_DOWN) {
		playerYVelocity = PLAYER_VELOCITY;
	} else {
		playerYVelocity = 0;
	}
	if (playerXVelocity != 0 || playerYVelocity != 0) {
		playerAnimState++;
	} else {
		playerAnimState = 0;
	}

	if (currentPadState & PAD_B) {
		playerXVelocity <<= 1;
		playerYVelocity <<= 1;
	}

	if (playerX > SCREEN_EDGE_RIGHT) {
		if (playerOverworldPosition < LEVEL_WIDTH*LEVEL_HEIGHT) {
			playerOverworldPosition += LEVEL_WIDTH;
			playerX = SCREEN_EDGE_LEFT+5;
			// TODO: Cool ppu scrolling anim
			gameState = GAME_STATE_REDRAW;
		} else if (!(playerXVelocity & 0x80)) {
			playerXVelocity = 0;
		}
	} else if (playerX < SCREEN_EDGE_LEFT) {
		if (playerOverworldPosition >= LEVEL_WIDTH) {
			playerOverworldPosition -= LEVEL_WIDTH;
			playerX = SCREEN_EDGE_RIGHT-5;
			gameState = GAME_STATE_REDRAW;
		} else if (playerXVelocity & 0x80) {
			playerXVelocity = 0;
		}

	}

	if (playerY > SCREEN_EDGE_BOTTOM) {
		if (playerOverworldPosition < LEVEL_WIDTH*LEVEL_HEIGHT) {
			++playerOverworldPosition;
			playerY = SCREEN_EDGE_TOP+5;
			// TODO: Cool ppu scrolling anim
			gameState = GAME_STATE_REDRAW;
		} else if (!(playerYVelocity & 0x80)) {
			playerYVelocity = 0;
		}
	} else if (playerY < SCREEN_EDGE_TOP) {
		if (playerOverworldPosition > 0) {
			--playerOverworldPosition;
			playerY = SCREEN_EDGE_BOTTOM-5;
			gameState = GAME_STATE_REDRAW;
		} else if (playerYVelocity & 0x80) {
			playerYVelocity = 0;
		}

	}

	playerX += playerXVelocity;
	playerY += playerYVelocity;

	currentSpriteId = PLAYER_SPRITE_ID;
	scratch = PLAYER_SPRITE_TILE + ((playerAnimState & 0x04) >> 1);
	currentSpriteId = oam_spr(playerX, playerY, scratch, 0, PLAYER_SPRITE_ID);
	currentSpriteId = oam_spr(playerX+8, playerY, scratch+1, 0, currentSpriteId);
	currentSpriteId = oam_spr(playerX, playerY+8, scratch+0x10, 0, currentSpriteId);
	currentSpriteId = oam_spr(playerX+8, playerY+8, scratch+0x11, 0, currentSpriteId);
}