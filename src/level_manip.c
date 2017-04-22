#include "src/globals.h"
#include "lib/neslib.h"
#pragma rodataseg ("ROM_01")
#pragma codeseg ("ROM_01")

void banked_draw_level() {
	ppu_off();
	vram_inc(0);
	vram_adr(NAMETABLE_A);

	for (i = 0; i < sizeof(screenBuffer); i++)
		screenBuffer[i] = 0;
	j = -1; 
	// And unpack into vram
	for (i = 0; i != 192; ++i) {
		// TODO: This is like a whirlwind of awful and inefficient. At the very least move this logic to another bank to make me feel better
		scratch = (currentLevel[i] & 0x3f) << 1; // Skip top 2 bytes. That's our palette bytes.
		scratchInt
	 = ((i / 16) << 6) + ((i % 16) << 1);
		vram_adr(NAMETABLE_A+scratchInt
	);
		vram_put(scratch);
		vram_put(scratch+1);
		vram_adr(NAMETABLE_A + scratchInt + 32);
		vram_put(scratch + 16);
		vram_put(scratch + 17);
		// FIXME: Palette bytes
		scratch = (currentLevel[i] & 0xc0); // Top 2 bits are the palette bytes... but where do they need to go?
		if (i % 32 == 16) 
			j -= 8;
		if (i % 2 == 0) 
			j++;
		if (i % 2 == 0) {
			// Even/left
			if ((i / 16) % 2 == 0) {
				// top
				scratch >>= 6;
			} else {
				//bottom
				scratch >>= 2;
			}
		} else {
			// Odd/right
			if ((i / 16) % 2 == 0) {
				// Top
				scratch >>= 4;
			} else {
				// Bottom 
				scratch >>= 0;
			}
		}
		screenBuffer[j] += scratch;
		//scratchInt = NAMETABLE_A+0x3c0+(i>>);
		vram_adr(NAMETABLE_A + scratchInt>>2);
	}
	vram_adr(NAMETABLE_A + 0x3c0);
	vram_write(screenBuffer, 0x30);
	//vram_fill(2, 0x3c0);
	vram_fill(0, 0x30);
	ppu_on_all();
}