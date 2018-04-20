// ***************************************************************************
// vectrex cartridge init block - edit to set game title
// ***************************************************************************

#include <vectrex.h>

// ---------------------------------------------------------------------------

struct cartridge_t
{
	char copyright[11];			// copyright string, must start with "g GCE" and must end with "\x80"
	const void* music;			// 16 bit memory adress of title music data
	signed int title_height;	// signed 8 bit value, height of game title letters
	unsigned int title_width;	// unsigned 8 bit value, width of game title letters
	int title_y;				// signed 8 bit value, y coordinate of game title
	int title_x;				// signed 8 bit value, x coordinate of game title
	char title[]; 				// game title string, must end with "\x80\x00"
};

// ---------------------------------------------------------------------------

const struct cartridge_t game_header __attribute__((section(".cartridge"), used)) = 
{
	.copyright 		= "g GCE 2018\x80",
	.music 			= &Vec_Music_1,
	.title_height 	= -8,
	.title_width 	= 80,
	.title_y 		= 16,
	.title_x 		= -65,
	.title 			= "PYORO-CHAN\x80" // \x00 automatically appended!
};

// ***************************************************************************
// end of file
// ***************************************************************************
