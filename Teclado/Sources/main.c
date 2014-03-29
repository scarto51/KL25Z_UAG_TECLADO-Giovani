
#include "derivative.h" /* include peripheral declarations */
#include "UART.h"
#include "LCD.h"
#include "Keyboard.h"

int main(void)

{
	vfnUART_Config();
	vfnLCD_Init();
	
	for(;;)
	{
		vfnKeyboard();
	}
	
	return 0;
}

