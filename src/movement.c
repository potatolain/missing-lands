#include "src/globals.h"
#include "lib/neslib.h"
#include "src/movement.h"
#include "src/sprites.h"
#pragma rodataseg ("ROM_01")
#pragma codeseg ("ROM_01")


void do_banked_movement() {
	int maxVelocity = (PLAYER_MAX_VELOCITY/2);
	#if DEBUG
		if (staticPadState & PAD_START && currentPadState & PAD_SELECT) {
			gameState = GAME_STATE_LEVEL_COMPLETE;
			return;
		}
	#endif

	if (currentPadState & PAD_B) {
		maxVelocity = PLAYER_MAX_VELOCITY;
	}

	if (staticPadState & PAD_START) {
		gameState = GAME_STATE_PAUSE;
		sfx_play(SFX_PAUSE, 2);
		return; // Don't do anything else.
	}
	if (!playerVelocityLockTime) {
		if (currentPadState & PAD_RIGHT) {
			if (playerXVelocity < maxVelocity) {
				playerXVelocity += PLAYER_VELOCITY_ACCEL;
			} else if (playerXVelocity > maxVelocity) {
				playerXVelocity -= PLAYER_VELOCITY_ACCEL;
			}
		} else if (currentPadState & PAD_LEFT) {
			if (abs(playerXVelocity) < maxVelocity) {
				playerXVelocity -= PLAYER_VELOCITY_ACCEL;
			} else if (abs(playerXVelocity) > maxVelocity) {
				playerXVelocity += PLAYER_VELOCITY_ACCEL;
			}
		} else if (playerXVelocity != 0) {
			if (playerXVelocity > 0) {
				playerXVelocity -= PLAYER_VELOCITY_ACCEL*2;
			} else {
				playerXVelocity += PLAYER_VELOCITY_ACCEL*2;
			}
		}

		if (currentPadState & PAD_UP) {
			if (abs(playerYVelocity) < maxVelocity) {
				playerYVelocity -= PLAYER_VELOCITY_ACCEL;
			} else if (abs(playerYVelocity) > maxVelocity) {
				playerYVelocity += PLAYER_VELOCITY_ACCEL;
			}
		} else if (currentPadState & PAD_DOWN) {
			if (playerYVelocity < maxVelocity) {
				playerYVelocity += PLAYER_VELOCITY_ACCEL;
			} else if (playerYVelocity > maxVelocity) {
				playerYVelocity -= PLAYER_VELOCITY_ACCEL;
			}
		} else if (playerYVelocity != 0) {
			if (playerYVelocity > 0) {
				playerYVelocity -= PLAYER_VELOCITY_ACCEL*2;
			} else {
				playerYVelocity += PLAYER_VELOCITY_ACCEL*2;
			}
			if (abs(playerYVelocity) == 1) {
				playerYVelocity = 0;
			}
		}
		
		if (playerXVelocity != 0 || playerYVelocity != 0) {
			playerAnimState++;
		} else {
			playerAnimState = 0;
		}

	} else {
		--playerVelocityLockTime;
	}
	if (playerInvulnTime)
		--playerInvulnTime;

	if ((playerX>>2) > SCREEN_EDGE_RIGHT) {
		playerOverworldPosition += LEVEL_WIDTH;
		playerX = (SCREEN_EDGE_LEFT+5)<<2;
		// TODO: Cool ppu scrolling anim
		gameState = GAME_STATE_WORLD_MOVEMENT;
		return;
	} else if ((playerX>>2) < SCREEN_EDGE_LEFT) {
		playerOverworldPosition -= LEVEL_WIDTH;
		playerX = (SCREEN_EDGE_RIGHT-5)<<2;
		gameState = GAME_STATE_WORLD_MOVEMENT;
		return;
	}

	if ((playerY>>2) > SCREEN_EDGE_BOTTOM) {
		++playerOverworldPosition;
		playerY = (SCREEN_EDGE_TOP+5)<<2;
		// TODO: Cool ppu scrolling anim
		gameState = GAME_STATE_WORLD_MOVEMENT;
		return;
	} else if ((playerY>>2) < SCREEN_EDGE_TOP) {
		--playerOverworldPosition;
		playerY = (SCREEN_EDGE_BOTTOM-5)<<2;
		gameState = GAME_STATE_WORLD_MOVEMENT;
		return;

	}

	// Okay, collision test time. Hold onto yer butts!
	// If you're trying to parse this, I'm seriously sorry. I probably couldn't even help you by the time you read this...
	if (playerYVelocity != 0) {
		scratch = (playerY + playerYVelocity)>>2;
		scratch2 = playerX>>2;
		if (playerYVelocity & 0x80) {
			// TL || TR
			if (test_collision(currentLevel[(scratch2>>4)+((((scratch)>>4))<<4)], 1) || test_collision(currentLevel[((scratch2+PLAYER_WIDTH)>>4)+(((scratch>>4))<<4)], 1)) {
				playerYVelocity = 0;
			}
			if (playerVelocityLockTime == 0)
				playerDirection = PLAYER_DIRECTION_UP;
		} else {
			// BL || BR
			if (test_collision(currentLevel[((scratch2)>>4)+((((scratch+PLAYER_HEIGHT)>>4))<<4)], 1) || test_collision(currentLevel[((scratch2+PLAYER_WIDTH)>>4)+((((scratch+PLAYER_HEIGHT)>>4))<<4)], 1)) {
				playerYVelocity = 0;
			}
			if (playerVelocityLockTime == 0)
				playerDirection = PLAYER_DIRECTION_DOWN;
		}
	}

	if (playerXVelocity != 0) {
		scratch = (playerX + playerXVelocity)>>2;
		scratch2 = playerY>>2;
		if (playerXVelocity & 0x80) {
			// TL || BL
			if (test_collision(currentLevel[(scratch>>4)+((((scratch2)>>4))<<4)], 1) || test_collision(currentLevel[(scratch>>4)+((((scratch2+PLAYER_HEIGHT)>>4))<<4)], 1)) {
				playerXVelocity = 0;
			}
			if (playerVelocityLockTime == 0)
				playerDirection = PLAYER_DIRECTION_LEFT;
		} else {
			// TR || BR
			if (test_collision(currentLevel[((scratch+PLAYER_WIDTH)>>4)+(((scratch2>>4))<<4)], 1) || test_collision(currentLevel[((scratch+PLAYER_WIDTH)>>4)+((((scratch2+PLAYER_HEIGHT)>>4))<<4)], 1)) {
				playerXVelocity = 0;
			}
			if (playerVelocityLockTime == 0)
				playerDirection = PLAYER_DIRECTION_RIGHT;
		}
	}

	do_sprite_collision();

	playerX += playerXVelocity;
	playerY += playerYVelocity;
	scratch = playerX>>2;
	scratch2 = playerY>>2;

	currentSpriteId = PLAYER_SPRITE_ID;
	scratch3 = PLAYER_SPRITE_TILE + ((playerAnimState & 0x04) >> 1) + playerDirection;
	if (playerInvulnTime && FRAME_COUNTER & 0x02)
		scratch3 = PLAYER_SPRITE_EMPTY;
	currentSpriteId = oam_spr(scratch, scratch2, scratch3, 0, PLAYER_SPRITE_ID);
	currentSpriteId = oam_spr(scratch+8, scratch2, scratch3+1, 0, currentSpriteId);
	currentSpriteId = oam_spr(scratch, scratch2+8, scratch3+0x10, 0, currentSpriteId);
	currentSpriteId = oam_spr(scratch+8, scratch2+8, scratch3+0x11, 0, currentSpriteId);
}

void do_sprite_collision() {
	scratch = playerX>>2;
	scratch2 = playerY>>2;
	for (i = 0; i < 12; ++i) {
		scratch3 = (extendedSpriteData[(i<<2)+1] & SPRITE_SIZE_MASK) == SPRITE_SIZE_NORMAL ? 16 : 8; // TODO: this logic is probably simpler than needed
		// Yes, I'm directly reading values from OAM without so much as a #define. Shut up.
		scratch4 = *(char*)(0x200 + FIRST_ENEMY_SPRITE_ID+3 + (i<<4));
		scratch5 = *(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4));
		// Make enemies a touch smaller so they're less likely to be hit by accident.
		if (extendedSpriteData[(i<<2)] == SPRITE_TYPE_ENEMY) {
			scratch4 += 5;
			scratch5 += 5;
			scratch3 = 6;
		}

		if (scratch < scratch4 + scratch3 && scratch + PLAYER_WIDTH > scratch4 && 
			scratch2 < scratch5 + scratch3 && scratch2 + PLAYER_WIDTH > scratch5) {
			// When we collide... 

			switch (extendedSpriteData[(i<<2)]) {
				case SPRITE_TYPE_ENEMY: 
					if (playerInvulnTime > 0)
						break; // If you're already hurting, we'll be nice.
					// Else... not so much
					playerHealth--;
					if (playerHealth == 0)
						gameState = GAME_STATE_GAME_OVER;
					sfx_play(SFX_HURT, 0);
					update_hud();
					playerVelocityLockTime = ENEMY_VELOCITY_LOCK_TIME;
					playerInvulnTime = (ENEMY_VELOCITY_LOCK_TIME*2);
					if (playerXVelocity != 0)
						playerXVelocity = ((0u - playerXVelocity));
					if (playerYVelocity != 0)
						playerYVelocity = ((0u - playerYVelocity)); // Boink!

					// If you aren't moving... force it.
					if (playerXVelocity == 0 && playerYVelocity == 0)
						playerYVelocity = 4;

					// And then mother nature was like "slooooooow doooown"
					if (abs(playerXVelocity) > PLAYER_VELOCITY)
						playerXVelocity >>= 1;

					if (abs(playerYVelocity) > PLAYER_VELOCITY)
						playerYVelocity >>= 1;


					break;

				case SPRITE_TYPE_WORLD_PIECE:
					scratch = playerOverworldPosition;
					if (currentLevel[MAP_TILE_SIZE + 32 + i] != 0xf0) {
						scratch = currentLevel[MAP_TILE_SIZE + 32 + i];
					}
					currentWorldData[scratch] |= extendedSpriteData[(i<<2)+3];
					sfx_play(SFX_EARTH_CHANGE, 0);
					// Now hide it.
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4))) = 0xff;
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4)+4)) = 0xff;
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4)+8)) = 0xff;
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4)+12)) = 0xff;

					worldChunkCount++;
					update_hud();
					// Please try not to vomit. I need 2 screen updates, so I trigger one after the other, and wait for nmi so they don't step on eachother.
					// NOTE: This blip is actually kinda informative - let's always show it.
					// if (scratch == playerOverworldPosition || worldChunkCount == worldTotalChunks) {
						ppu_wait_nmi();
						gameState = GAME_STATE_REDRAW;
					// }


					
					break;
				case SPRITE_TYPE_HEART:
					if (playerHealth < 5)
						playerHealth++;
		
					// Mario toad voice "Baaaaii"
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4))) = 0xff;
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4)+4)) = 0xff;
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4)+8)) = 0xff;
					(*(char*)(0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4)+12)) = 0xff;
					if (i < 8)
						world_sprite_state[playerOverworldPosition] |= BYTE_TO_BIT[i];

					sfx_play(SFX_HEART, 1);
					update_hud();



					break;

				default: 
					// Eh, do nothing. It shall live on.
					break;
			}
		}

	}
}