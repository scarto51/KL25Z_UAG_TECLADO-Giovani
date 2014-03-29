/*
 * Keyboard.c
 *
 *  Created on: Mar 12, 2014
 *      Author: ANTONIO
 */

#include "derivative.h"
#include "Keyboard.h"
#include "LCD.h"

char gbReceive;
char gbWrite  = 0;
char gbRelease = 0;
char gbMayus  = 0;
char gbShift  = 0;
char gbComand = 0;
int gbCount[8]={0,0,0,0,0,0,0,0};
int  gbPage =0;
char gbChangePage = 0;

int gbOffset = 0;



char gabID[49]= 
{
	0x0E,0x16,0x1E,0x26,0x25,0x2E,0x36,0x3D,0x3E,0x46,0x45,0x4E,0x55,
	0x15,0x1D,0x24,0x2D,0x2C,0x35,0x3C,0x43,0x44,0x4D,0x54,0x5B,
	0x1C,0x1B,0x23,0x2B,0x34,0x33,0x3B,0x42,0x4B,0x4C,0x52,0x5D,
	0x61,0x1A,0x22,0x21,0x2A,0x32,0x31,0x3A,0x41,0x49,0x4A,0x29
};

char gabMin[49]=
{
	"|1234567890'¿qwertyuiop´+asdfghjklñ{}<zxcvbnm,.- "
};

char gabMay[49]=
{
	"|1234567890'¿QWERTYUIOP´+ASDFGHJKLÑ{}<ZXCVBNM,.- "	
};

char gabShiftMin[49]=
{
	0xDF,'!','"','#','$','%','&','/','(',')','=','?','¡',
	'Q','W','E','R','T','Y','U','I','O','P','¨','*',
	'A','S','D','F','G','H','J','K','L','Ñ','[',']',
	'>','Z','X','C','V','B','N','M',';',':','_',' '
};

char gabShiftMay[49]=
{
	0xDF,'!','"','#','$','%','&','/','(',')','=','?','¡',
	'q','w','e','r','t','y','u','i','o','p','¨','*',
	'a','d','d','f','g','h','j','k','l','ñ','[',']',
	'>','z','x','c','v','b','n','m',';',':','_',' '
};

char gmPages[8][32]=
{
		{"                                "},
		{"                                "},
		{"                                "},
		{"                                "},
		{"                                "},
		{"                                "},
		{"                                "},
		{"                                "}
};



void vfnKeyboard(void)
{
	if(gbWrite)
	{
		if(gbCount[gbPage]<32)
		{	
			while((gbReceive  !=  gabID[gbOffset]) &&  (gbOffset<49))
			{
				gbOffset++;
			}
			
			if(gbMayus&&gbShift)
			{
				vfnLCD_Write(CHAR,gabShiftMay[gbOffset]);
				gmPages[gbPage][gbCount[gbPage]]= gabShiftMay[gbOffset];
			}
			
			else if(gbMayus)
			{
				vfnLCD_Write(CHAR,gabMay[gbOffset]);
				gmPages[gbPage][gbCount[gbPage]]= gabMay[gbOffset];
			}
			
			else if(gbShift)
			{
				vfnLCD_Write(CHAR,gabShiftMin[gbOffset]);
				gmPages[gbPage][gbCount[gbPage]]= gabShiftMin[gbOffset];
			}
			
			else
			{
				vfnLCD_Write(CHAR,gabMin[gbOffset]);
				gmPages[gbPage][gbCount[gbPage]]= gabMin[gbOffset];
			}
			gbCount[gbPage]++;
			if(gbCount[gbPage]==16)
			{
				vfnLCD_Write(INS,DOWN);
			}
			if(gbCount[gbPage]==32)
			{
				gbPage++;
				if(gbPage==8)
				{
					gbPage = 0;
				}
				vfnLCD_Write(INS,UP);
				vfnLCD_Write_Array(16,&gmPages[gbPage][0]);
				vfnLCD_Write(INS,DOWN);
				vfnLCD_Write_Array(16,&gmPages[gbPage][16]);
				if(gbCount[gbPage]<15)
				{
					vfnLCD_Write(INS,UP+gbCount[gbPage]);	
				}
				else
				{
					vfnLCD_Write(INS,DOWN+gbCount[gbPage]-16);
				}
			}
		}

		gbOffset = 0;
		gbWrite  = 0;
	}
	
	if(gbChangePage)
	{
		vfnLCD_Write(INS,UP);
		vfnLCD_Write_Array(16,&gmPages[gbPage][0]);
		vfnLCD_Write(INS,DOWN);
		vfnLCD_Write_Array(16,&gmPages[gbPage][16]);
		if(gbCount[gbPage]<15)
		{
			vfnLCD_Write(INS,UP+gbCount[gbPage]);	
		}
		else
		{
			vfnLCD_Write(INS,DOWN+gbCount[gbPage]-16);
		}
		
		gbChangePage =0;
	}
}



void UART0_IRQHandler(void)
{
	if(gbRelease)
	{
		gbRelease = 0;
		gbReceive = UART0_D;
		if((gbReceive == 0x12) || (gbReceive == 0x59))
		{
			gbShift = 0;
		}
	}
	else
	{
		gbReceive = UART0_D;
		
		if(gbReceive == 0xF0)
		{
			gbRelease = 1;
		}
		
		else if((gbReceive == 0x12) || (gbReceive == 0x59))
		{
			gbShift = 1;
		}
		
		else if(gbReceive == 0x58)
		{
			if(gbMayus)
			{
				gbMayus = 0;
			}
			
			else
			{
				gbMayus = 1;
			}
		}
		
		else if(gbReceive == 0x66)
		{
			if(gbCount[gbPage])
			{
				vfnLCD_Write(INS,BACKSPACE);
				vfnLCD_Write(CHAR,' ');
				vfnLCD_Write(INS,BACKSPACE);
				gbCount[gbPage]--;
				gmPages[gbPage][gbCount[gbPage]]= ' ';
			}
			
			if(gbCount[gbPage]==15)
			{
				vfnLCD_Write(INS,UP+15);
				vfnLCD_Write(CHAR,' ');
				vfnLCD_Write(INS,BACKSPACE);
				gmPages[gbPage][gbCount[gbPage]]= ' ';
			}
		}
		
		else if(gbReceive == 0x5A)
		{
			vfnLCD_Write(INS,DOWN);
			gbCount[gbPage] = 16;
		}
		
		else if(gbReceive == 0x05)
		{
			gbPage = 0;
			gbChangePage = 1;
		}
		
		else if(gbReceive == 0x06)
		{
			gbPage = 1;
			gbChangePage = 1;
		}
		
		else if(gbReceive == 0x04)
		{
			gbPage = 2;
			gbChangePage = 1;
		}
		
		else if(gbReceive == 0x0c)
		{
			gbPage = 3;
			gbChangePage = 1;
		}
		
		else if(gbReceive == 0x03)
		{
			gbPage = 4;
			gbChangePage = 1;
		}
		
		else if(gbReceive == 0x0B)
		{
			gbPage = 5;
			gbChangePage = 1;
		}
		
		else if(gbReceive == 0x83)
		{
			gbPage = 6;
			gbChangePage = 1;
		}
		
		else if(gbReceive == 0x0A)
		{
			gbPage = 7;
			gbChangePage = 1;
		}
		
		else
		{
			gbWrite=1;
		}
	}

}
