// ***************************************************************************
// bean
// ***************************************************************************

#include <vectrex.h>

#include "bean.h"
#include "ground.h"

// ---------------------------------------------------------------------------
// constant data for the bean's vectors

const int vectors_bean[] =
{
	0,0,-100,
	-1,100,100,
	-1,-100,100,
	-1,-100,-100,
	-1,100,-100,
	1
};

// ---------------------------------------------------------------------------
// look-up table of bean position
const int xpos[] = 
{
	-120,
	-104,
	-88,
	-72,
	-56,
	-40,
	-24,
	-8,
	8,
	24,
	40,
	56,
	72,
	88,
	104,
	120
	
};

// ---------------------------------------------------------------------------
// global variable for the bean's sprite

struct object bean =
{
	{
		{0, 0}	// coord.y, coord.x
		
	},
	0,	// lane
	2,	// speed
	10,	// scale
	0	// drawn
	
};

// ---------------------------------------------------------------------------
// function to draw the bean in the screen

void draw_bean()
{
	Reset0Ref();
	VIA_t1_cnt_lo = 110;
	Moveto_d(bean.coord.y,bean.coord.x);
	VIA_t1_cnt_lo = bean.scale;
	Draw_VLp(&vectors_bean);
	
}

// ---------------------------------------------------------------------------
// function to move the bean (falling)

void move_bean()
{
	bean.coord.y-=bean.speed;
	
}

// ---------------------------------------------------------------------------
// function to check the bean (hit the ground, etc)

void check_bean()
{	
	if(bean.coord.y < -110)
	{
		ground_state[bean.lane] = 0;
		bean.drawn = 0;
	}
}

// ---------------------------------------------------------------------------
// function to spawn a bean on the top end of the screen

void spawn_bean()
{
	bean.coord.y = 120;
	bean.lane = Random()%16;
	bean.coord.x = xpos[bean.lane];//xpos[bean.lane];
	bean.drawn = 1;
	
}

	
// ***************************************************************************
// end of file
// ***************************************************************************

