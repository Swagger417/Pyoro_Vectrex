// ***************************************************************************
// main
// ***************************************************************************

#include <vectrex.h>

#include "utils/controller.h"
#include "utils/print.h"

#include "pyoro.h"
#include "bean.h"
#include "ground.h"

// Notes
// Original pyoro walks on 1 or 2 tiles at the same time
// original pyoro has 30 tiles
// original pyoro very small
//

// ---------------------------------------------------------------------------
// at system startup, a cold reset is performed
// cold reset: the vectrex logo is shown, all ram data is cleared
// if the reset button is pressed, then a warm reset is performed
// warm reset: skip vectrex logo and keep ram data
// after each reset, the game title screen is shown and then main() is called

// start of program code

//int i;

void game_init()
{
	// activate first joystick, switch off second joystick
	enable_controller_1_x();
	enable_controller_1_y();
	disable_controller_2_x();
	disable_controller_2_y();
	
	init_pyoro();
	spawn_bean();
	init_ground();
	
}

// ---------------------------------------------------------------------------
// main game loop, this is where the action happens

void game_loop()
{
	int player_alive = 1;
	
	// as long as player is alive
	while(player_alive)
	{
		Wait_Recal();
		Intensity_5F();
		
		// move pyoro
		move_pyoro();
		
		if(bean.drawn)
		{	
			// move bean
			move_bean();
			
			// check ground collision
			check_bean();
			
			// draw bean
			draw_bean();
		}
		else
		{
			spawn_bean();
		}
		
		
		// draw pyoro
		draw_pyoro();
		
		// draw the ground
		draw_ground();
		
		//-----------------------------------------
		// Developer help lines and dots
		
		// Draw bean lines
		/*Reset0Ref();
		VIA_t1_cnt_lo = 110;
		Moveto_d(0,-120);
		for (i = 0; i < 16; i++)
		{
			Draw_Line_d(-120,0);
			Moveto_d(120,16);
		}*/
		
		// draw(0|0) dot
		/*Reset0Ref();
		VIA_t1_cnt_lo = 110;
		Moveto_d(0, 0);
		Dot_here();*/
		
		// draw bottom line
		/*Reset0Ref();
		VIA_t1_cnt_lo = 110;
		Moveto_d(-120, -128);
		Draw_Line_d(0,127);
		Draw_Line_d(0,127);*/
		
		//-----------------------------------------
		
		// check for collisions
		player_alive = check_pyoro();
	}
}



// ---------------------------------------------------------------------------
// R.I.P.

void game_over()
{
	// show good bye message for 3 seconds
	// maybe more like original
	unsigned int t = 150;
	while(--t > 0)
	{
		Wait_Recal();
		Reset0Ref();
		print_str(0, -70, "GAME OVER");
	}
}

int main(void)
{	
	// local variables
	int error_code = 0;
	
	// main loop
	do
	{
		//game_options();
		game_init();
		game_loop();
		game_over();
		
	}
	while (1);
	
	// if error_code is <= 0, then a warm reset will be performed,
	// otherwise a cold reset will be performed,
	// after the reset, the game title screen appears,
	// then main() is called
	return error_code;
}		
		
// ***************************************************************************
// end of file
// ***************************************************************************
