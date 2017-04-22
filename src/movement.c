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
			return;
		} else if (!(playerXVelocity & 0x80)) {
			playerXVelocity = 0;
		}
	} else if (playerX < SCREEN_EDGE_LEFT) {
		if (playerOverworldPosition >= LEVEL_WIDTH) {
			playerOverworldPosition -= LEVEL_WIDTH;
			playerX = SCREEN_EDGE_RIGHT-5;
			gameState = GAME_STATE_REDRAW;
			return;
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
			return;
		} else if (!(playerYVelocity & 0x80)) {
			playerYVelocity = 0;
		}
	} else if (playerY < SCREEN_EDGE_TOP) {
		if (playerOverworldPosition > 0) {
			--playerOverworldPosition;
			playerY = SCREEN_EDGE_BOTTOM-5;
			gameState = GAME_STATE_REDRAW;
			return;
		} else if (playerYVelocity & 0x80) {
			playerYVelocity = 0;
		}

	}

	// Okay, collision test time. Hold onto yer butts!
	// If you're trying to parse this, I'm seriously sorry. I probably couldn't even help you by the time you read this...
	if (playerYVelocity != 0) {
		scratch = playerY + playerYVelocity;
		if (playerYVelocity & 0x80) {
			// TL || TR
			if (test_collision(currentLevel[(playerX>>4)+((((scratch)>>4))<<4)]) || test_collision(currentLevel[((playerX+PLAYER_WIDTH)>>4)+(((scratch>>4))<<4)])) {
				playerYVelocity = 0;
			}
			playerDirection = PLAYER_DIRECTION_UP;
		} else {
			// BL || BR
			if (test_collision(currentLevel[((playerX)>>4)+((((scratch+PLAYER_HEIGHT)>>4))<<4)]) || test_collision(currentLevel[((playerX+PLAYER_WIDTH)>>4)+((((scratch+PLAYER_HEIGHT)>>4))<<4)])) {
				playerYVelocity = 0;
			}
			playerDirection = PLAYER_DIRECTION_DOWN;
		}
	}

	if (playerXVelocity != 0) {
		scratch = playerX + playerXVelocity;
		if (playerXVelocity & 0x80) {
			// TL || BL
			if (test_collision(currentLevel[(scratch>>4)+((((playerY)>>4))<<4)]) || test_collision(currentLevel[(scratch>>4)+((((playerY+PLAYER_HEIGHT)>>4))<<4)])) {
				playerXVelocity = 0;
			}
			playerDirection = PLAYER_DIRECTION_LEFT;
		} else {
			// TR || BR
			if (test_collision(currentLevel[((scratch+PLAYER_WIDTH)>>4)+(((playerY>>4))<<4)]) || test_collision(currentLevel[((scratch+PLAYER_WIDTH)>>4)+((((playerY+PLAYER_HEIGHT)>>4))<<4)])) {
				playerXVelocity = 0;
			}
			playerDirection = PLAYER_DIRECTION_RIGHT;
		}
	}



	playerX += playerXVelocity;
	playerY += playerYVelocity;

	currentSpriteId = PLAYER_SPRITE_ID;
	scratch = PLAYER_SPRITE_TILE + ((playerAnimState & 0x04) >> 1) + playerDirection;
	currentSpriteId = oam_spr(playerX, playerY, scratch, 0, PLAYER_SPRITE_ID);
	currentSpriteId = oam_spr(playerX+8, playerY, scratch+1, 0, currentSpriteId);
	currentSpriteId = oam_spr(playerX, playerY+8, scratch+0x10, 0, currentSpriteId);
	currentSpriteId = oam_spr(playerX+8, playerY+8, scratch+0x11, 0, currentSpriteId);
}