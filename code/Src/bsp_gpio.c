#include"bsp_gpio.h"
#include"GUI.h"
#include"gpio.h"

void GPIO_Init(void)
{
	// _pac1 = 1;
	// _pac3 = 1;
	// _pac4 = 1;
	// _pcc7 = 1;
	_pac1 = 0;
	_pac4=1;

	PA03_SetCurrent(CURRENT_LEVEL3);
	//led1,2
	_phc0 = 0;
	_phc1 = 0;
	_pa1 = 0;

	// _pfc0 = 0;
	// _pfc1 = 0;

	_int2s1 = 1;
	_int2s0 = 0;

	_int2f = 0;
	_int2e = 1;
	
	led1_off();
	led2_off();
	//_iecc = 0b11001010;
}


