#include "src/globals.h"
#include "lib/neslib.h"
#include "src/movement.h"
#include "src/sprites.h"
#pragma rodataseg ("ROM_01")
#pragma codeseg ("ROM_01")

void do_banked_movement() {
	if (!playerVelocityLockTime) {
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
	} else {
		playerVelocityLockTime--;
	}

	if (playerX > SCREEN_EDGE_RIGHT) {
		if (playerOverworldPosition < LEVEL_WIDTH*LEVEL_HEIGHT) {
			playerOverworldPosition += LEVEL_WIDTH;
			playerX = SCREEN_EDGE_LEFT+5;
			// TODO: Cool ppu scrolling anim
			gameState = GAME_STATE_WORLD_MOVEMENT;
			return;
		} else if (!(playerXVelocity & 0x80)) {
			playerXVelocity = 0;
		}
	} else if (playerX < SCREEN_EDGE_LEFT) {
		if (playerOverworldPosition >= LEVEL_WIDTH) {
			playerOverworldPosition -= LEVEL_WIDTH;
			playerX = SCREEN_EDGE_RIGHT-5;
			gameState = GAME_STATE_WORLD_MOVEMENT;
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
			gameState = GAME_STATE_WORLD_MOVEMENT;
			return;
		} else if (!(playerYVelocity & 0x80)) {
			playerYVelocity = 0;
		}
	} else if (playerY < SCREEN_EDGE_TOP) {
		if (playerOverworldPosition > 0) {
			--playerOverworldPosition;
			playerY = SCREEN_EDGE_BOTTOM-5;
			gameState = GAME_STATE_WORLD_MOVEMENT;
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
			if (playerVelocityLockTime == 0)
				playerDirection = PLAYER_DIRECTION_UP;
		} else {
			// BL || BR
			if (test_collision(currentLevel[((playerX)>>4)+((((scratch+PLAYER_HEIGHT)>>4))<<4)]) || test_collision(currentLevel[((playerX+PLAYER_WIDTH)>>4)+((((scratch+PLAYER_HEIGHT)>>4))<<4)])) {
				playerYVelocity = 0;
			}
			if (playerVelocityLockTime == 0)
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
			if (playerVelocityLockTime == 0)
				playerDirection = PLAYER_DIRECTION_LEFT;
		} else {
			// TR || BR
			if (test_collision(currentLevel[((scratch+PLAYER_WIDTH)>>4)+(((playerY>>4))<<4)]) || test_collision(currentLevel[((scratch+PLAYER_WIDTH)>>4)+((((playerY+PLAYER_HEIGHT)>>4))<<4)])) {
				playerXVelocity = 0;
			}
			if (playerVelocityLockTime == 0)
				playerDirection = PLAYER_DIRECTION_RIGHT;
		}
	}

	do_sprite_collision();

	playerX += playerXVelocity;
	playerY += playerYVelocity;

	currentSpriteId = PLAYER_SPRITE_ID;
	scratch = PLAYER_SPRITE_TILE + ((playerAnimState & 0x04) >> 1) + playerDirection;
	if (playerVelocityLockTime && FRAME_COUNTER & 0x01)
		scratch = PLAYER_SPRITE_EMPTY;
	currentSpriteId = oam_spr(playerX, playerY, scratch, 0, PLAYER_SPRITE_ID);
	currentSpriteId = oam_spr(playerX+8, playerY, scratch+1, 0, currentSpriteId);
	currentSpriteId = oam_spr(playerX, playerY+8, scratch+0x10, 0, currentSpriteId);
	currentSpriteId = oam_spr(playerX+8, playerY+8, scratch+0x11, 0, currentSpriteId);
}

void do_sprite_collision() {
	scratch = playerX + playerXVelocity;
	scratch2 = playerY + playerYVelocity;
	for (i = 0; i < 12; ++i) {
		scratch3 = extendedSpriteData[(i<<2)+1] == SPRITE_SIZE_NORMAL ? 16 : 8; // TODO: this logic is probably simpler than needed
		// Yes, I'm directly reading values from OAM without so much as a #define. Shut up.
		scratch4 = *(char*)(0x200 + FIRST_ENEMY_SPRITE_ID+3 + (i<<4));
		scratch5 = *(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4));
		// Make enemies a touch smaller so they're less likely to be hit by accident.
		if (extendedSpriteData[(i<<2)] == SPRITE_TYPE_ENEMY) {
			scratch4 += 2;
			scratch5 += 2;
			scratch3 -= 4;
		}

		if (playerX < scratch4 + scratch3 && playerX + PLAYER_WIDTH > scratch4 && 
			playerY < scratch5 + scratch3 && playerY + PLAYER_WIDTH > scratch5) {
			// When we collide... 

			switch (extendedSpriteData[(i<<2)]) {
				case SPRITE_TYPE_ENEMY: 
					if (playerVelocityLockTime > 0)
						break; // If you're already hurting, we'll be nice.
					// Else... not so much
					playerHealth--;
					if (playerHealth == 0)
						gameState = GAME_STATE_GAME_OVER;
					update_hud();
					playerVelocityLockTime = ENEMY_VELOCITY_LOCK_TIME;
					if (playerXVelocity != 0)
						playerXVelocity = ((0 - playerXVelocity) >> 1) || 1;
					if (playerYVelocity != 0)
						playerYVelocity = ((0 - playerXVelocity) >> 1) || 1; // Boink!

					break;

				case SPRITE_TYPE_WORLD_PIECE:
					scratch = playerOverworldPosition;
					if (currentLevel[MAP_TILE_SIZE + 32 + i] != 0xf0) {
						scratch = currentLevel[MAP_TILE_SIZE + 32 + i];
					}
					currentWorldData[scratch] |= extendedSpriteData[(i<<2)+3];
					
					// Now hide it.
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4))) = 0xff;
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4)+4)) = 0xff;
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4)+8)) = 0xff;
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4)+12)) = 0xff;

					worldChunkCount++;
					update_hud();
					// Please try not to vomit. I need 2 screen updates, so I trigger one after the other, and wait for nmi so they don't step on eachother.
					if (scratch == playerOverworldPosition) {
						ppu_wait_nmi();
						gameState = GAME_STATE_REDRAW;
					}


					
					break;
				default: 
					// Eh, do nothing. It shall live on.
					break;
			}
		}

	}
}