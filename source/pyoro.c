// ***************************************************************************
// pyoro
// ***************************************************************************

#include <vectrex.h>

#include "utils/controller.h"

#include "pyoro.h"
#include "types.h"
#include "bean.h"
#include "ground.h"

// ---------------------------------------------------------------------------
// constant data for pyoro's vectors

const int vectors_pyoro_right[] =
{
	0,0,-50,
	-1,-50,50,
	-1,50,50,
	-1,100,-100,
	-1,50,50,
	-1,-50,70,	
	-1,-100,-120,
	1
};

const int vectors_pyoro_left[] =
{
	0,0,50,
	-1,-50,-50,
	-1,50,-50,
	-1,100,100,
	-1,50,-50,
	-1,-50,-70,	
	-1,-100,120,
	1
};

// ---------------------------------------------------------------------------
// look-up table of the left lane borders
const int lane_borders[] = 
{
	-120,	//Lane 0	from -120 to -113
	-112,	//Lane 1	from -112 to -97
	-96,	//Lane 2	from -96  to -81
	-80,	//Lane 3	from -80  to -65
	-64,	//Lane 4	from -64  to -49
	-48,	//Lane 5	from -48  to -33
	-32,	//Lane 6	from -32  to -17
	-16,	//Lane 7	from -16  to -1
	0,		//Lane 8	from  0   to  15
	16,		//Lane 9	from  16  to  31
	32,		//Lane 10	from  32  to  47
	48,		//Lane 12	from  48  to  63
	64,		//Lane 13	from  64  to  79
	80,		//Lane 14	from  80  to  95
	96,		//Lane 15	from  96  to  111
	112,	//Lane 16	from  112 to  120
	121		//Most right border 120 + 1 due to way of internal calculation
};

// ---------------------------------------------------------------------------
// distance variables initialization

int distance_x = 0;
int distance_y = 0;

// ---------------------------------------------------------------------------
// global variable for pyoro's sprite

struct player pyoro =
{
	{
		{0, 0}	// coord.y, coord.x
		
	},
	8,		// lane
	3,		// speed
	RIGHT,	// direction
	20		// scale
	// union mit : 1;
};

// ---------------------------------------------------------------------------
// function to set pyoro's default values

void init_pyoro()
{
	pyoro.coord.y = -120;
	pyoro.coord.x = 0;
	pyoro.lane = 8;
	pyoro.direction = RIGHT;
	
}

// ---------------------------------------------------------------------------
// function to draw pyoro in the screen

void draw_pyoro()
{
	Reset0Ref();
	VIA_t1_cnt_lo = 110;
	Moveto_d(pyoro.coord.y,pyoro.coord.x);
	VIA_t1_cnt_lo = pyoro.scale;
	
	if(pyoro.direction)
	{
		Draw_VLp(&vectors_pyoro_right);
	}
	else
	{
		Draw_VLp(&vectors_pyoro_left);
	}
	
	//play shooting animation
	/*
	Reset0Ref();
	VIA_t1_cnt_lo = 110;
	Moveto_d(pyoro.coord.y,pyoro.coord.x);
	Draw_Line_d(127,pyoro.direction?127:-127);
	Draw_Line_d(127,pyoro.direction?127:-127);
	*/
	
	// Developer help, draw line which resembles pyoro's current lane
	/*
	Reset0Ref();
	VIA_t1_cnt_lo = 110;
	Moveto_d(0,0);
	Draw_Line_d(0,(int)pyoro.lane*5);
	*/
}

// ---------------------------------------------------------------------------
// control pyoro with joystick 1

//no moving while walking
void move_pyoro()	//maybe multiple different movement functions instead of using pyoro.speed
{
	// read joystick values
	check_joysticks();
	
	// only x movement
	if (joystick_1_left() && pyoro.coord.x > -120)
	{
		pyoro.coord.x -= pyoro.speed;	// move pyoro to the left
		pyoro.direction = LEFT;			// set the direction pyoro faces
		
		if(pyoro.coord.x < lane_borders[pyoro.lane])	// if pyoro walked onto another lane
		{
			if(ground_state[pyoro.lane-1])	// 
			{
				--pyoro.lane;
			}
			else
			{
				pyoro.coord.x = lane_borders[pyoro.lane];
			}
		}
		
	}
	else if (joystick_1_right() && pyoro.coord.x < 120)
	{
		pyoro.coord.x += pyoro.speed;
		pyoro.direction = RIGHT;
		
		if(pyoro.coord.x >= lane_borders[pyoro.lane+1])
		{
			if(ground_state[pyoro.lane+1])
			{
				++pyoro.lane;
			}
			else
			{
				pyoro.coord.x = lane_borders[pyoro.lane+1]-1;
			}
		}
		
	}
	else
	{
		// read buttons
		check_buttons();


		// shooting
		if (button_1_4_pressed())
		{
			//play shooting animation
			Reset0Ref();
			VIA_t1_cnt_lo = 110;
			Moveto_d(pyoro.coord.y,pyoro.coord.x);
			Draw_Line_d(127,pyoro.direction?127:-127);
			Draw_Line_d(127,pyoro.direction?127:-127);
			
			//shoot bean
			if(pyoro.direction)
			{
				distance_x = (bean.coord.x>>1) - (pyoro.coord.x>>1);
				if(distance_x > 0)
				{
					distance_y = (bean.coord.y>>1) + 55;
					if((distance_x > distance_y) && (distance_x < distance_y+10))
					{
						bean.drawn = 0;
					}
				}
			}
			else if(!pyoro.direction)
			{
				distance_x = (bean.coord.x>>1) - (pyoro.coord.x>>1);
				if(distance_x < 0)
				{
					distance_y = (bean.coord.y>>1) + 55;
					if((-distance_x > distance_y) && (-distance_x < distance_y+10))
					{
						bean.drawn = 0;
					}
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------
// function to check if pyoro died

int check_pyoro()
{
	if(bean.coord.y < -90)
	{
		/*
		if((pyoro.coord.x < bean.coord.x+7) && (pyoro.coord.x > bean.coord.x-7))
		{
			return 0;
		}
		*/
		if (pyoro.lane == bean.lane)
		{
			return 0;
		}
	}
	else
	{
		return 1;
	}
	return 1;
}


/*BACKUP No shooting while walking

// ---------------------------------------------------------------------------
// control pyoro with joystick 1

//no moving while walking
void move_pyoro()	//maybe multiple different movement functions instead of using pyoro.speed
{
	// read joystick values
	check_joysticks();
	
	// only x movement
	if (joystick_1_left() && pyoro.coord.x > -120)
	{
		pyoro.coord.x -= pyoro.speed;	// move pyoro to the left
		pyoro.direction = LEFT;			// set the direction pyoro faces
		
		if(pyoro.coord.x < lane_borders[pyoro.lane])	// if pyoro walked onto another lane
		{
			if(ground_state[pyoro.lane-1])	// 
			{
				--pyoro.lane;
			}
			else
			{
				pyoro.coord.x = lane_borders[pyoro.lane];
			}
		}
		
	}
	else if (joystick_1_right() && pyoro.coord.x < 120)
	{
		pyoro.coord.x += pyoro.speed;
		pyoro.direction = RIGHT;
		
		if(pyoro.coord.x >= lane_borders[pyoro.lane+1])
		{
			if(ground_state[pyoro.lane+1])
			{
				++pyoro.lane;
			}
			else
			{
				pyoro.coord.x = lane_borders[pyoro.lane+1]-1;
			}
		}
		
	}
	else
	{
		// read buttons
		check_buttons();


		// shooting
		if (button_1_4_pressed())
		{
			//play shooting animation
			Reset0Ref();
			VIA_t1_cnt_lo = 110;
			Moveto_d(pyoro.coord.y,pyoro.coord.x);
			Draw_Line_d(127,pyoro.direction?127:-127);
			Draw_Line_d(127,pyoro.direction?127:-127);
			
			//shoot bean
			if(pyoro.direction)
			{
				distance_x = (bean.coord.x>>1) - (pyoro.coord.x>>1);
				if(distance_x > 0)
				{
					distance_y = (bean.coord.y>>1) + 55;
					if((distance_x > distance_y) && (distance_x < distance_y+10))
					{
						bean.drawn = 0;
					}
				}
			}
			else if(!pyoro.direction)
			{
				distance_x = (bean.coord.x>>1) - (pyoro.coord.x>>1);
				if(distance_x < 0)
				{
					distance_y = (bean.coord.y>>1) + 55;
					if((-distance_x > distance_y) && (-distance_x < distance_y+10))
					{
						bean.drawn = 0;
					}
				}
			}
		}
	}
}

*/

/*BACKUP Shooting while walking with shord standing

// ---------------------------------------------------------------------------
// control pyoro with joystick 1

//no moving while walking
void move_pyoro()	//maybe multiple different movement functions instead of using pyoro.speed
{
	// read buttons
	check_buttons();


	// shooting
	if (button_1_4_pressed())
	{
		//play shooting animation
		Reset0Ref();
		VIA_t1_cnt_lo = 110;
		Moveto_d(pyoro.coord.y,pyoro.coord.x);
		Draw_Line_d(127,pyoro.direction?127:-127);
		Draw_Line_d(127,pyoro.direction?127:-127);
		
		//shoot bean
		if(pyoro.direction)
		{
			distance_x = (bean.coord.x>>1) - (pyoro.coord.x>>1);
			if(distance_x > 0)
			{
				distance_y = (bean.coord.y>>1) + 55;
				if((distance_x > distance_y) && (distance_x < distance_y+10))
				{
					bean.drawn = 0;
				}
			}
		}
		else if(!pyoro.direction)
		{
			distance_x = (bean.coord.x>>1) - (pyoro.coord.x>>1);
			if(distance_x < 0)
			{
				distance_y = (bean.coord.y>>1) + 55;
				if((-distance_x > distance_y) && (-distance_x < distance_y+10))
				{
					bean.drawn = 0;
				}
			}
		}
	}
	else
	{
		// read joystick values
		check_joysticks();
		
		// only x movement
		if (joystick_1_left() && pyoro.coord.x > -120)
		{
			pyoro.coord.x -= pyoro.speed;	// move pyoro to the left
			pyoro.direction = LEFT;			// set the direction pyoro faces
			
			if(pyoro.coord.x < lane_borders[pyoro.lane])	// if pyoro walked onto another lane
			{
				if(ground_state[pyoro.lane-1])	// 
				{
					--pyoro.lane;
				}
				else
				{
					pyoro.coord.x = lane_borders[pyoro.lane];
				}
			}
			
		}
		else if (joystick_1_right() && pyoro.coord.x < 120)
		{
			pyoro.coord.x += pyoro.speed;
			pyoro.direction = RIGHT;
			
			if(pyoro.coord.x >= lane_borders[pyoro.lane+1])
			{
				if(ground_state[pyoro.lane+1])
				{
					++pyoro.lane;
				}
				else
				{
					pyoro.coord.x = lane_borders[pyoro.lane+1]-1;
				}
			}
		}
	}
}
*/

 
	
// ***************************************************************************
// end of file
// ***************************************************************************

