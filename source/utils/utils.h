// ***************************************************************************
// utility functions
// ***************************************************************************

#pragma once
#include <vectrex.h>
#include "sound.h"

// ---------------------------------------------------------------------------
// scale factor used for all absolute sprite coordinates

#define GRID_SCALE 0x7F

// ---------------------------------------------------------------------------
// position vector beam at absolute sprite coordinates

static inline __attribute__((always_inline))
void reset_beam(void)
{
	//Reset0Ref();
	dp_VIA_cntl = 0xcc;	// zero the integrators
}

static inline __attribute__((always_inline)) 
void scale_beam(const unsigned int scale)
{
	dp_VIA_t1_cnt_lo = scale;
}

static inline __attribute__((always_inline)) 
void position_beam(const int y, const int x)
{
	reset_beam();
	scale_beam(GRID_SCALE);
	Moveto_d(y, x);
}

static inline __attribute__((always_inline)) 
void position_beam_yx(const long int yx)
{
	reset_beam();
	scale_beam(GRID_SCALE);
	Moveto_dd(yx);
}

static inline __attribute__((always_inline)) 
void position_beam_s(const int y, const int x, const unsigned int scale)
{
	reset_beam();
	scale_beam(scale);
	Moveto_d(y, x);
}

static inline __attribute__((always_inline)) 
void position_beam_yxs(const long int yx, const unsigned int scale)
{
	reset_beam();
	scale_beam(scale);
	Moveto_dd(yx);
}

// ---------------------------------------------------------------------------

static inline __attribute__((always_inline)) 
void sync()
{

	// game loop header start - do not change
	DP_to_C8();
	Explosion_Snd(current_explosion);
	Init_Music_chk(current_music);
	Wait_Recal();
	Do_Sound();
	// game loop header end
}

void Sync();

// ---------------------------------------------------------------------------
// workaround: gcc6809 cannot handle shift by non-constant int (long int works)

static inline __attribute__((always_inline)) 
unsigned int shift_left(unsigned int operand, unsigned int bits)
{ 
	return (unsigned int) ((long unsigned int) operand << (long unsigned int) bits);
}

static inline __attribute__((always_inline)) 
unsigned int shift_right(unsigned int operand, unsigned int bits)
{ 
	return (unsigned int) ((long unsigned int) operand >> (long unsigned int) bits);
}

static inline __attribute__((always_inline)) 
int rotate_left(int operand, unsigned int bits)
{ 
	return (int) ((long int) operand << (long unsigned int) bits);
}

static inline __attribute__((always_inline)) 
int roate_right(int operand, unsigned int bits)
{ 
	return (int) ((long int) operand >> (long unsigned int) bits);
}

// ---------------------------------------------------------------------------
// workaround: gcc6809 cannot handle division by non-constant int (long int works)

static inline __attribute__((always_inline)) 
unsigned int divu(unsigned int dividend, unsigned int divisor)
{ 
	return (unsigned int) ((long unsigned int) dividend / (long unsigned int) divisor);
}

static inline __attribute__((always_inline)) 
int divs(int dividend, int divisor)
{ 
	return (int) ((long int) dividend / (long int) divisor);
}

// ***************************************************************************
// end of file
// ***************************************************************************
