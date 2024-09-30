#pragma bss-name(push, "ZEROPAGE")

#include "lib/neslib.h"
#include "lib/nesdoug.h" 
#include "res/map_forest_outpost.h"
#include "res/map_world.h"

// there's some oddities in the palette code, black must be 0x0f, white must be 0x30
#define BLACK 0x0f
#define DK_GY 0x00
#define LT_GY 0x10
#define WHITE 0x30

// GLOBAL VARIABLES
// ^ all variables should be global for speed, zeropage global is even faster

const char palette_bg[] =
{
	BLACK, DK_GY, LT_GY, WHITE,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0
}; 

const char palette_spr[] =
{
	BLACK, DK_GY, LT_GY, WHITE,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0
}; 

const unsigned char text[] = "A LITTLE GUY IN A NES GAME";

struct Vector2
{
	unsigned char x;
	unsigned char y;
};

// bitmask for player gamepad input
char _input1;
struct Vector2 _playerPosition = { 8, 7 };
unsigned char _playerMoveSpeed = 1;

// PROTOTYPES

// background draw calls
void DrawBG();

// main game loop calls
void UpdateGameState();
void DrawGameState();

// game logic
void GetInput();
void UpdatePlayer();

// draw state methods
void DrawPlayer();

void main (void) 
{	
	// screen off
	ppu_off();
	
	// load the palettes
	pal_bg(palette_bg);
	pal_spr(palette_spr);

	// use the second set of tiles for sprites
	// both bg and sprite are set to 0 by default
	bank_spr(1);

	DrawBG();

	
	// turn on screen
	ppu_on_all();
		
	// infinite loop
	while (1)
	{
		// wait till beginning of the frame
		ppu_wait_nmi();
		// the sprites are pushed from a buffer to the OAM during nmi
		// clear all sprites from sprite buffer
		oam_clear();

		UpdateGameState();
		DrawGameState();
	}
}

void UpdateGameState()
{
	GetInput();
	UpdatePlayer();
}

void DrawGameState()
{
	DrawPlayer();
}

void DrawBG()
{
	// load the text
	// set a start position for the text
	// vram_write draws the array to the screen
	//vram_adr(NTADR_A(2,2));
	//vram_write(text, sizeof(text));

	vram_adr(NAMETABLE_A);
	// this sets a start position on the BG, top left of screen
	// vram_adr() and vram_unrle() need to be done with the screen OFF
	
	vram_unrle(map_world);
	// this unpacks an rle compressed full nametable
	// created by NES Screen Tool
}

void GetInput()
{
	_input1 = pad_poll(0); // read the first controller
}

unsigned char _stepDelay;

void UpdatePlayer()
{
	char source = _input1;
	
	char moveLeft  = source & PAD_LEFT;
	char moveRight = source & PAD_RIGHT;
	char moveUp    = source & PAD_UP;
	char moveDown  = source & PAD_DOWN;
	
	if (_stepDelay != 5)
	{
		++_stepDelay;
		return;
	}
	_stepDelay = 0;

	if (moveLeft)  
	{
		_playerPosition.x -= 8;
	}
	
	if (moveRight) 
	{
		_playerPosition.x += 8;
	}
	
	if (moveUp)    
	{
		_playerPosition.y -= 8;
	}
	
	if (moveDown)  
	{
		_playerPosition.y += 8;
	}
	
	
}

void DrawPlayer()
{
	// push a single sprite
	// oam_spr(unsigned char x,unsigned char y,unsigned char chrnum,unsigned char attr);
	// use tile #0, palette #0
	oam_spr(_playerPosition.x, _playerPosition.y, 1, 0);
}