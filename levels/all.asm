.export _lvl_details

.segment "ROM_02"
	_lvl_details: ; NOTE: This only exists here, but applies for all banks. DON'T CHANGE THE SIZE w/o considering this
	; Start location; tileId, x, y, Number of chunks
	.byte 27, 80, 80, 3
	.include "levels/processed/lvl1_tiles.asm"
.segment "ROM_03"
	.byte 27, 80, 80, 2
	.include "levels/processed/lvl2_tiles.asm"
.segment "ROM_04"
	.byte 27, 80, 80, 4
	.include "levels/processed/lvl3_tiles.asm"
.segment "ROM_05"
	.byte 27, 80, 80, 3
	.include "levels/processed/lvl4_tiles.asm"
.segment "ROM_06"
	.byte 27, 80, 80, 5
	.include "levels/processed/lvl5_tiles.asm"
.segment "ROM_07"
	.byte 31, 80, 40, 9
	.include "levels/processed/lvl6_tiles.asm"