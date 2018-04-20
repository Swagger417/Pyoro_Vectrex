// ***************************************************************************
// vector
// ***************************************************************************

#pragma once

// ---------------------------------------------------------------------------
// data structure describing a single vector

struct vector_t
{
	int y; 		// relative y coordinate
	int x; 		// relative x coordinate
};


// ---------------------------------------------------------------------------
// enum type for sight direction of player
enum direction_t
{
	LEFT = 0,
	RIGHT = 1
};

// ---------------------------------------------------------------------------
// data structure describing an object

struct object
{
	union
	{
		struct
		{
			int y;
			int x;
		};
		long int yx;
	}coord;
	
	unsigned int lane;
	int speed;
	const unsigned int scale;
	int drawn;
};

// ---------------------------------------------------------------------------
// data structure describing pyoro

struct player
{
	union
	{
		struct
		{
			int y;
			int x;
		};
		long int yx;
	}coord;
	
	unsigned int lane;
	int speed;
	enum direction_t direction;
	const unsigned int scale;
};











// ***************************************************************************
// end of file
// ***************************************************************************
