#include "HT66F2390.h"
#include"main.h"
#include"uart.h"
#include"bsp_gpio.h"
#include"AT_CMD.h"
#include"OLED.h"
#include"iic.h"
#include"delay.h"
#include"EEPROM.h"
#include"gpio.h"

typedef struct{
	uint8 windspeed;
	uint8 tem;
	uint8 mode;
	uint8 on;
}ControllerStruct;

ControllerStruct Controller;

uint8 controller_data[3] = {0x0B,0x00,0x00};
uint8 controller_rx_flag = 0;
// uint8 controller_counter = 0;

void SysTickInit(void);
void TimInit(void);

uint16 RecCnt = 0;

uint8 RecDataBuf[4];
// uint8 rx_data[128];

#define APPROXIMATE(value,low,high) approxiate(value,low,high)

inline uint8 approxiate(uint8 value,uint8 low,uint8 high)
{
	return value>low&&value<high;
}

uint8 RecFlag = 0;
uint8 TraFlag = 0;
uint8 tx_data[3];
uint8 esp_rx_flag = 0;

void main()
{
	SysTickInit();
	_emi = 1;

	UART_Init();
	GPIO_Init();
	ESP_Init();
	TimInit();

	while(1){
		if(esp_rx_flag==1){
			if(tx_data[0]!=0xA0){
				controllerSend(tx_data);
				memcpy(controller_data,tx_data,3);
			}
			ESP_Send(controller_data,3);
			esp_rx_flag=0;
		}
		if(controller_rx_flag==1){
			delay_ms(50);
			controllerSend(controller_data);
			ESP_Send(controller_data,3);
			controller_rx_flag = 0;
		}
		delay_ms(50);
	}
}

void TimInit(void)
{
	_ptm2c0 = 0b00000000;
	_ptm2c1 = 0b11000000;

	//38000HZ
	uint16 comparator_p =27648;
	_ptm2rpl = comparator_p&0xFF;
	_ptm2rph = comparator_p>>8;

	_ptm2pe = 1;
	_ptm2pf = 0;
	_mf2f = 0;
	_mf2e = 1;

	_pt2on = 0;
	
}

void SysTickInit(void)
{
	// _hirc1 = 0;
	// _hirc0 = 0;
	// _hircen = 1;
	// while(_hircf!=1);

	_pbs1 = 0xf0;
	_hxtm = 1;
	_hxten = 1;
	while(_hxtf!=1);
	_fhs = 1;
	//?????????¡¤
	_wdtc = 0b10101000;
}