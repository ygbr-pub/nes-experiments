/*	simple Hello World, for cc65, for NES
 *  writing to the screen with rendering disabled
 *	using neslib
 *	Doug Fraker 2018
 */	

#include "LIB/neslib.h"
#include "LIB/nesdoug.h" 

#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30
#define ALPHA 0x28
// there's some oddities in the palette code, black must be 0x0f, white must be 0x30
 
#pragma bss-name(push, "ZEROPAGE")

// GLOBAL VARIABLES
// all variables should be global for speed
// zeropage global is even faster
const unsigned char text[]="A LITTLE GUY IN A NES GAME"; // zero terminated c string

const unsigned char palette_bg[]={
	BLACK, DK_GY, LT_GY, WHITE, // black, gray, lt gray, white
	0,0,0,0,
	0,0,0,0,
	0,0,0,0
	}; 

const unsigned char palette_spr[]={
	BLACK, BLACK, DK_GY, LT_GY,  // black, black, yellow
	0,0,0,0,
	0,0,0,0,
	0,0,0,0
	}; 

struct Vector2
{
	unsigned int x;
	unsigned int y;
};

unsigned char pad1;
struct Vector2 _starPosition = { 0x88, 0x88 };

void main (void) 
{	
	ppu_off(); // screen off
	
	// load the palettes
	pal_bg(palette_bg);
	pal_spr(palette_spr);

	// use the second set of tiles for sprites
	// both bg and sprite are set to 0 by default
	bank_spr(1);

	// load the text
	// set a start position for the text
	// vram_write draws the array to the screen
	vram_adr(NTADR_A(2,2));
	vram_write(text,sizeof(text));
	
	ppu_on_all(); // turn on screen
		
	
	while (1)
	{
		// infinite loop

		ppu_wait_nmi(); // wait till beginning of the frame
		// the sprites are pushed from a buffer to the OAM during nmi
		
		// clear all sprites from sprite buffer
		oam_clear();

		pad1 = pad_poll(0); // read the first controller

		if(pad1 & PAD_LEFT)
		{
			_starPosition.x -= 1;
		}
		else if (pad1 & PAD_RIGHT)
		{
			_starPosition.x += 1;
		}
		if(pad1 & PAD_UP)
		{
			_starPosition.y -= 1;
		}
		else if (pad1 & PAD_DOWN)
		{
			_starPosition.y += 1;
		}
		
		// push a single sprite
		// oam_spr(unsigned char x,unsigned char y,unsigned char chrnum,unsigned char attr);
		// use tile #0, palette #0
		oam_spr(_starPosition.x, _starPosition.y, 0, 0);
	}
}