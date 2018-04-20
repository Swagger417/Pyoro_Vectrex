// ***************************************************************************
// print
// ***************************************************************************

#include <vectrex.h>

// ---------------------------------------------------------------------------
// print a c string (with \0 at the end) at absolute coordinates (y, x)

void print_str(int y, int x, char* text)
{
	volatile char message[20];
	message[0] = (char) y;
	message[1] = (char) x;
	unsigned int i = 2;
	do
	{
		message[i++] = *text;
	}
	while (*(text++) != 0);	
	message[i - 1] = '\x80';
	Reset0Ref_D0();
	Print_Str_yx((void*) &message[0]);
}

// ---------------------------------------------------------------------------
// print decimal unsigned int value at absolute coordinates (y, x)

void print_int(int y, int x, unsigned int z)
{
	volatile char message[6];
	message[0] = (char) y;
	message[1] = (char) x;
	message[5] = '\x80';
	unsigned int i = 4;
	do
	{
		message[i--] = (char) ('0' + z % 10);
		z = z / 10;
	}
	while (i > 1);	
	Reset0Ref_D0();
	Print_Str_yx((void*) &message[0]);
}

// ---------------------------------------------------------------------------
// print binary unsigned int value at absolute coordinates (y, x)

void print_bin(int y, int x, unsigned int z)
{
	volatile char message[11];
	message[0] = (char) y;
	message[1] = (char) x;
	message[10] = '\x80';
	unsigned int i = 9;
	do
	{
		message[i--] = (char) ('0' + (z & 1));
		z >>= 1; 
	}
	while (i > 1);	
	Reset0Ref_D0();
	Print_Str_yx((void*) &message[0]);
}

// ---------------------------------------------------------------------------
// print decimal unsigned long int value at absolute coordinates (y, x)

void print_long_int(int y, int x, unsigned long int z)
{
	volatile char message[8];
	message[0] = (char) y;
	message[1] = (char) x;
	message[7] = '\x80';
	unsigned int i = 6;
	do
	{
		message[i--] = (char) ('0' + (z % 10));
		z = z / 10;
	}
	while (i > 1);	
	Reset0Ref_D0();
	Print_Str_yx((void*) &message[0]);
}

// ***************************************************************************
// end of file
// ***************************************************************************
