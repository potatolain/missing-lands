#include "src/globals.h"
#include "lib/neslib.h"
#include "src/sprites.h"

#pragma rodataseg ("ROM_01")
#pragma codeseg ("ROM_01")

const unsigned char sprite_data[] = {
	// World
	SPRITE_TYPE_WORLD_PIECE, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_1, 0x08, LEVEL_FRAGMENT_1,
	SPRITE_TYPE_WORLD_PIECE, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_2, 0x04, LEVEL_FRAGMENT_2,
	SPRITE_TYPE_ENEMY, SPRITE_SIZE_NORMAL | SPRITE_PALETTE_2, 0x40, SPRITE_DIRECTION_DOWN
};

void banked_draw_sprites() {
	for (i = 0; i < 12 && currentLevel[MAP_TILE_SIZE + (i<<1)] != 255; ++i) {
		scratch = currentLevel[MAP_TILE_SIZE + (i<<1)];
		scratch2 = currentLevel[MAP_TILE_SIZE + (i<<1)+1];

		extendedSpriteData[(i<<2)  ] = sprite_data[(scratch2<<2)];
		extendedSpriteData[(i<<2)+1] = sprite_data[(scratch2<<2)+1] & SPRITE_SIZE_MASK;
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
