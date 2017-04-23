#include "src/globals.h"
#include "lib/neslib.h"
#include "src/sprites.h"

#pragma rodataseg ("ROM_01")
#pragma codeseg ("ROM_01")

static char sprite_directions[12];
static char sprite_direction_time[12];

const unsigned char sprite_data[] = {
	// World
	SPRITE_TYPE_WORLD_PIECE, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_1, 0x08, LEVEL_FRAGMENT_1,
	SPRITE_TYPE_WORLD_PIECE, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_1, 0x08, LEVEL_FRAGMENT_2,
	// Turtle
	SPRITE_TYPE_ENEMY, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_2 | SPRITE_ANIM_FULL, 0x40, SPRITE_DIRECTION_DOWN,
	// Crab
	SPRITE_TYPE_ENEMY, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_3 | SPRITE_ANIM_DEFAULT, 0x80, 0,
	// Slime
	SPRITE_TYPE_ENEMY, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_0 | SPRITE_ANIM_DEFAULT, 0x84, 0,
	// Killer tomato
	SPRITE_TYPE_ENEMY, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_3 | SPRITE_ANIM_DEFAULT, 0x84, 0,
	// Skeleton Skeletron
	SPRITE_TYPE_ENEMY, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_3 | SPRITE_ANIM_DEFAULT, 0x88, 0
};

void banked_draw_sprites() {
	for (i = 0; i < 12; i++) {
		sprite_directions[i] = SPRITE_DIRECTION_UNDEF;
		sprite_direction_time[i] = 0;
	}

	for (i = 0; i < 12 && currentLevel[MAP_TILE_SIZE + (i<<1)] != 255; ++i) {
		scratch = currentLevel[MAP_TILE_SIZE + (i<<1)];
		scratch2 = currentLevel[MAP_TILE_SIZE + (i<<1)+1];

		extendedSpriteData[(i<<2)  ] = sprite_data[(scratch2<<2)];
		extendedSpriteData[(i<<2)+1] = sprite_data[(scratch2<<2)+1];
		extendedSpriteData[(i<<2)+2] = sprite_data[(scratch2<<2)+2];
		extendedSpriteData[(i<<2)+3] = sprite_data[(scratch2<<2)+3];

		// Scratch 1 is the position. Need to do some hax to get it to 16x16 coords
		scratch2 = (scratch >> 4) << 4; // Y
		scratch = ((scratch % 16) << 4); // X
		scratch3 = extendedSpriteData[(i<<2)+2];
		// I'm so, so sorry.
		scratch4 = sprite_data[((currentLevel[MAP_TILE_SIZE + (i<<1)+1])<<2)+1] & SPRITE_PALETTE_MASK;

		// Logic to hide these if you've already collected it.
		scratch5 = playerOverworldPosition;
		if (currentLevel[MAP_TILE_SIZE + 32 + i] != 0xf0) {
			scratch5 = currentLevel[MAP_TILE_SIZE + 32 + i];
		}
		if (currentWorldData[scratch5] & extendedSpriteData[(i<<2)+3]) {
			scratch2 = 0xf0;
		}


		oam_spr(scratch, scratch2, scratch3, scratch4, FIRST_ENEMY_SPRITE_ID + (i << 4));
		oam_spr(scratch+8, scratch2, scratch3+1, scratch4, (FIRST_ENEMY_SPRITE_ID + 4) + (i << 4));
		oam_spr(scratch, scratch2+8, scratch3+0x10, scratch4, (FIRST_ENEMY_SPRITE_ID + 8) + (i << 4));
		currentSpriteId = oam_spr(scratch+8, scratch2+8, scratch3+0x11, scratch4, (FIRST_ENEMY_SPRITE_ID + 12) + (i << 4));
	}
	oam_hide_rest(currentSpriteId);
}

void banked_update_sprites() {
	for (i = 0; i < 12; i++) {
		// Skipping the nice oam functions here because... well, they do more than I want right now.
		if (*(unsigned char*)(0x200 + FIRST_ENEMY_SPRITE_ID+(i<<4)) == NESLIB_SPRITE_GONE)
			continue;

		scratchInt = (0x200 + FIRST_ENEMY_SPRITE_ID + (i<<4));
		if ((extendedSpriteData[(i<<2)+1] & SPRITE_ANIM_MASK) == SPRITE_ANIM_DEFAULT) {
			scratch = extendedSpriteData[(i<<2)+2];
			scratch += (FRAME_COUNTER & 0x10) ? 0 : 2;
			*(unsigned char*)(scratchInt+1) = scratch;
			*(unsigned char*)(scratchInt+5) = scratch+1;
			*(unsigned char*)(scratchInt+9) = scratch+0x10;
			*(unsigned char*)(scratchInt+13) = scratch+0x11;
		} else if ((extendedSpriteData[(i<<2)+1] & SPRITE_ANIM_MASK) == SPRITE_ANIM_FULL && sprite_directions[i] != SPRITE_DIRECTION_UNDEF) {
			scratch = extendedSpriteData[(i<<2)+2];
			scratch += (FRAME_COUNTER & 0x10) ? 0 : 2;
			if (sprite_directions[i] != SPRITE_DIRECTION_UNDEF)
				scratch += sprite_directions[i];
			else
				scratch += SPRITE_DIRECTION_DOWN;
			*(unsigned char*)(scratchInt+1) = scratch;
			*(unsigned char*)(scratchInt+5) = scratch+1;
			*(unsigned char*)(scratchInt+9) = scratch+0x10;
			*(unsigned char*)(scratchInt+13) = scratch+0x11;
		}
		++sprite_direction_time[i];

		// If you're an enemy, we can move you around! Imagine that...
		if (extendedSpriteData[i<<2] == SPRITE_TYPE_ENEMY) {
			scratch = scratch4 = *(unsigned char*)(scratchInt+3); // X
			scratch2 = scratch5 = *(unsigned char*)(scratchInt); // Y

			// scratch3 is whether to switch direction... 
			if (sprite_direction_time[i] > SPRITE_MIN_DIRECTION_TIME && (rand8() & 0x0f) == 1) {
				switch (rand8() % 5) {
					case 0:
						scratch3 = SPRITE_DIRECTION_LEFT;
						break;
					case 1:
						scratch3 = SPRITE_DIRECTION_RIGHT;
						break;
					case 2:
						scratch3 = SPRITE_DIRECTION_UP;
						break;
					case 3:
						scratch3 = SPRITE_DIRECTION_DOWN;
						break;
					default: 
						scratch3 = SPRITE_DIRECTION_UNDEF;

				}
				sprite_directions[i] = scratch3;
				sprite_direction_time[i] = 0;
			}

			if (sprite_directions[i] != SPRITE_DIRECTION_UNDEF) {
				switch (sprite_directions[i]) {
					case SPRITE_DIRECTION_LEFT:
						scratch4 -= SPRITE_MOVEMENT_SPEED;
						if (scratch4 < SCREEN_EDGE_LEFT || test_collision(currentLevel[(scratch4>>4)+((((scratch5)>>4))<<4)], 0) || test_collision(currentLevel[(scratch4>>4)+((((scratch5+SPRITE_HEIGHT)>>4))<<4)], 0)) {
							scratch4 = scratch;
						}

						break;
					case SPRITE_DIRECTION_RIGHT:
						scratch4 += SPRITE_MOVEMENT_SPEED;
						if (scratch4 > SCREEN_EDGE_RIGHT || test_collision(currentLevel[((scratch4+SPRITE_WIDTH)>>4)+(((scratch5>>4))<<4)], 0) || test_collision(currentLevel[((scratch4+SPRITE_WIDTH)>>4)+((((scratch5+SPRITE_HEIGHT)>>4))<<4)], 0)) {
							scratch4 = scratch;
						}

						break;
					case SPRITE_DIRECTION_UP:
						scratch5 -= SPRITE_MOVEMENT_SPEED;
						if (scratch5 < SCREEN_EDGE_TOP || test_collision(currentLevel[(scratch4>>4)+((((scratch5)>>4))<<4)], 0) || test_collision(currentLevel[((scratch4+SPRITE_WIDTH)>>4)+(((scratch5>>4))<<4)], 0)) {
							scratch5 = scratch2;
						}
						break;
					case SPRITE_DIRECTION_DOWN:
						scratch5 += SPRITE_MOVEMENT_SPEED;
						if (scratch5 > SCREEN_EDGE_BOTTOM || test_collision(currentLevel[((scratch4)>>4)+((((scratch5+SPRITE_HEIGHT)>>4))<<4)], 0) || test_collision(currentLevel[((scratch4+SPRITE_WIDTH)>>4)+((((scratch5+SPRITE_HEIGHT)>>4))<<4)], 0)) {
							scratch5 = scratch2;
						}

						break;
				}

			}
			

			*(unsigned char*)(scratchInt) = scratch5;
			*(unsigned char*)(scratchInt+4) = scratch5;
			*(unsigned char*)(scratchInt+8) = scratch5+8;
			*(unsigned char*)(scratchInt+12) = scratch5+8;

			*(unsigned char*)(scratchInt+3) = scratch4;
			*(unsigned char*)(scratchInt+7) = scratch4+8;
			*(unsigned char*)(scratchInt+11) = scratch4;
			*(unsigned char*)(scratchInt+15) = scratch4+8;

		}


	}
}