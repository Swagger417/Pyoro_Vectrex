// ***************************************************************************
// bean
// ***************************************************************************

#include <vectrex.h>

#include "types.h"

#include "ground.h"

// ---------------------------------------------------------------------------
// global variable of the ground's state
int ground_state[] = 
{
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1
	
};
// maybe something like packet type of johannsen

// ---------------------------------------------------------------------------
// function to set the ground's default values

void init_ground()
{
	ground_state[0] = 1;
	ground_state[1] = 1;
	ground_state[2] = 1;
	ground_state[3] = 1;
	ground_state[4] = 1;
	ground_state[5] = 1;
	ground_state[6] = 1;
	ground_state[7] = 1;
	ground_state[8] = 1;
	ground_state[9] = 1;
	ground_state[10] = 1;
	ground_state[11] = 1;
	ground_state[12] = 1;
	ground_state[13] = 1;
	ground_state[14] = 1;
	ground_state[15] = 1;
	
}


// ---------------------------------------------------------------------------
// function to draw the ground in the screen

void draw_ground()
{
	int x = 0;
	
	Reset0Ref();
	VIA_t1_cnt_lo = 110;
	Moveto_d(-120, -128);
	VIA_t1_cnt_lo = 16;
	
	for(x = 0; x < 16; ++x)	//unroll later
	{
		ground_state[x] ? Draw_Line_d(0,110) : Moveto_d(0,110);
	}

}
