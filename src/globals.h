// This file defines globals that can be used all over. You'll want common things in here, as the NES has very, very
// limited ram. 
extern unsigned char currentPadState;
extern unsigned char i, j, scratch, scratch2; 
extern unsigned int scratchInt;
extern unsigned char currentLevel[256];
extern char screenBuffer[0x30];


extern void put_str(unsigned int adr, const char *str);
extern void clear_screen();