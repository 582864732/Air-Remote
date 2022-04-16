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
	uint8 wind_velocity;
	uint8 tem;
	uint8 mode;
	uint8 on;
}ControllerStruct;

ControllerStruct Controller;

void SysTickInit(void);
void TimInit(void);

uint16 RecCnt = 0;

RecDataBuf[4];
uint8 RecBuf = 0;
uint8 CurrentBuf = 0;
// uint8 rx_data[128];

#define RISING_EDGE 0x10
#define FALLING_EDGE 0x20
#define SET_RISEING_EDGE() 	_integ&=~0x30;_integ|=0x10
#define SET_FALLING_EDGE() 	_integ&=~0x30;_integ|=0x20
#define GET_EDGE() (_integ&0x30) 

#define APPROXIMATE(value,low,high) approxiate(value,low,high)

inline uint8 approxiate(uint8 value,uint8 low,uint8 high)
{
	return value>low&&value<high;
}

uint8 RecFlag = 0;
uint8 TraFlag = 0;
uint8 tx_data[4] = {0xb2,~0xb2,0x1F,~0x1F};


uint8 cnts[100];
uint16 c = 0;
void main()
{
	SysTickInit();
	_emi = 1;

	UART_Init();
	GPIO_Init();
	IIC_Init();
	// oled_init();
	// ESP_Init();
	TimInit();
	uint8 i=0;
	// _pf0 = 0;
	// _pt2on=1;
	// EEPROM_WriteBytes(0,"hell",4);
	// EEPROM_WriteByte(0x00,'H');
	// uint8 data;
	// // _eed='i';
	// //  = EEPROM_ReadBytes(0);
	// uint16 i=0;
	// for(;i<7;i++){
	// 	data = EEPROM_ReadByte(i);
	// 	// oled_show_char(0,i,data)
	// 	oled_show_string(32,i,toString(_eeal),0);
	// 	oled_show_string(0,i,toString(data),0);
	// }
	// oled_show_char(0,0,data);

	uint16 n = 0;
	while(1){
		// UART_SendString(toString(100));
		// UART_SendString("hello\n");
		if(RecFlag==1){
			// ESP_Send(RecDataBuf[RecBuf],6);
			// UART_SendByte('\n');
			uint8 i=0;
			for(;i<4;i++){
				UART_SendByte('\n');
				UART_SendString(toString(RecDataBuf[i]));
			}
			UART_SendByte('\n');

			c=0;
			_int2e=1;
			RecFlag=0;
		}
		// n++;
		// if(n==290) n=0;
		// _ptm3al = n&0xFF;
		// _ptm3ah = n>>8;
		// delay_ms(100);
		
		// _pt3on = 0;
		// _pt3on=1;
		// while(_pt3on);
		// delay_ms(1000);
	}
}

void TimInit(void)
{
	_ptm2c0 = 0b00010000;
	_ptm2c1 = 0b11000000;

	//38000HZ
	uint16 comparator_p = 145;
	_ptm2rpl = comparator_p&0xFF;
	_ptm2rph = comparator_p>>8;

	_ptm2pe = 1;
	_ptm2pf = 0;
	_mf2f = 0;
	_mf2e = 1;

	_pt2on = 0;
	

	_ptm3c0 = 0b00010000;
	_ptm3c1 = 0b11000000;

	comparator_p = 145;
	_ptm3rpl = comparator_p&0xFF;
	_ptm3rph = comparator_p>>8;

	_mf4f = 0;
	_mf4e = 1;
	_ptm3pf = 0;
	_ptm3pe = 1;
	_pt3on = 0;

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
	//³ýÄÜ¿´ÃÅ¹·
	_wdtc = 0b10101000;
}

uint8 last_state = 0;
void __attribute((interrupt(0x30))) GPIO_Interrupt(void)
{
	// led1_toggle();
	// static uint8 n=0;
	if(_pt2on == 0){
		_pt2on = 1;
		last_state = 0;
		_int2e = 0;
		uint8 i=0;
		for(;i<4;i++){
			RecDataBuf[i] = 0;
		}
		RecCnt = 0;
	}
	_int2f = 0;
}

uint8 switch_mode = 0;
uint8 unpack_index = 0;
uint8 unpack_bit = 0;
uint8 stop_code = 140;
uint8 unpackData(uint8 cnt)
{
	static uint8 switch_mode = 0;
	// oled_show_string(0,1,toString(cnt),0);
	// delay_ms(1000);
	switch(switch_mode){
		case 0:
			if(cnt>10&&last_state==0){
				switch_mode = 3;
			}else{
				switch_mode = 0;
				unpack_index = 0;
				unpack_bit = 0;
				return ERROR;
			}
			break;
		case 3:
		// UART_SendString(toString(cnt));
			if(cnt>10&&last_state==1){
				// led1_toggle();
				switch_mode = 1;
			}else{
				switch_mode = 0;
				unpack_index = 0;
				unpack_bit = 0;
				return ERROR;
			}
			break;
		case 1:
			if(last_state==0){
				if(cnt>30||cnt<5){
					switch_mode = 0;
					unpack_index = 0;
					unpack_bit = 0;
					return ERROR;
				}
				break;
			}else{
				if(cnt<30&&cnt>5){
					// RecDataBuf[unpack_index] &= (~1)<<(unpack_bit);
				}else{
					// led2_toggle();
					RecDataBuf[unpack_index] |= 1<<(unpack_bit);
				}
			}
			unpack_bit++;
			if(unpack_bit==8){
				unpack_bit=0;
				unpack_index++;
			}
			if(unpack_index == 4){
				// led1_toggle();
				switch_mode = 0;
				unpack_bit = 0;
				unpack_index = 0;
				return FINISH;
			}
			break;

		default:
			switch_mode = 0;
			unpack_index = 0;
			unpack_bit = 0;
			break;
	} 
	return OK;
}     

uint8 packData(uint8 *cnt)
{
	static uint8 switch_mode = 0;
	static uint8 int_cnt = 0;
	static uint8 lead_code1 = 230;
	static uint8 lead_code2 = 115;
	static uint8 lead_bit = 15;
	static uint8 one_bit = 45;
	static uint8 send_bit = 0;
	static uint8 stop_bit = 170;
	// oled_show_string(0,1,toString(cnt),0);
	// delay_ms(1000);
	switch(switch_mode){
		case 0:
			if(*cnt==lead_code1){
				*cnt = 0;
				switch_mode = 3;
			}
			break;
		case 3:
			if(*cnt==lead_code2){
				*cnt=0;
				switch_mode = 1;
			}
			break;
		case 1:
			// led2_toggle();
			if(*cnt==lead_bit){
				switch_mode = 2;
				*cnt = 0;
				send_bit = (tx_data[unpack_index]&(1<<unpack_bit))?one_bit:lead_bit;
			}
			break;
		case 2:
			if(*cnt==send_bit){
				*cnt=0;
				unpack_bit++;
				switch_mode=1;
				if(unpack_bit==8){
					unpack_bit=0;
					unpack_index++;
				}
				if(unpack_index == 4){
					switch_mode = 0;
					unpack_bit = 0;
					unpack_index = 0;
					return FINISH;
				}
			}
			break;
		// case 4:
		// 	if(*cnt==lead_bit){
		// 		*cnt=0;
		// 		switch_mode=5;
		// 	}
		// 	break;
		// case 5:
		// 	if(*cnt == stop_code){
		// 		*cnt = 0;
		// 		switch_mode = 0;
		// 		return FINISH;
		// 	}
	} 
	return OK;
}  

// uint8 IR_counter = 0;

void __attribute((interrupt(0x20))) PTM2_Interrupt(void)
{
	// led2_toggle();
	if(_ptm2pf == 1){
		RecCnt++;
		if(_pa4==0) _pa1^=1;
		else _pa1 = 0;
		if(last_state!=_pa4){
			cnts[c++] = RecCnt;
			uint8 state = unpackData(RecCnt);
			switch(state){
				// case OK:
				// 	// led2_on();
				// 	// _pt2on = 0;
				// 	// _int2e = 1;
				// 	_ptm2pf = 0;
				// 	RecCnt = 0;
				// 	return;
				case FINISH:
					_pt2on = 0;
					// _int2e = 1;
					RecCnt = 0;
					_ptm2pf = 0;
					RecFlag = 1;
					// led1_toggle();
					return;
			}
			last_state = _pa4;
			RecCnt = 0;
		}
		if(RecCnt>500){
			_pt2on = 0;
			RecCnt = 0;
			// RecFlag = 1;
			_int2e = 1;
		}
		_ptm2pf = 0;
	}
}

void __attribute((interrupt(0x38))) PTM3_Interrupt(void)
{
	static uint8 tx_cnt = 0;
	static uint8 state = 0;
	if(_ptm3pf){
		tx_cnt++;
		if( packData(&tx_cnt)==FINISH){
			_ptm3pf = 0;
			_pt3on = 0;
			tx_cnt = 0;
			state = 0;
			led1_toggle();
			return ;
		}
		if(tx_cnt==0) state^=1;
		if(state==0){
			_pa1^=1;
		}
		else{
			 _pa1 = 0;
			led2_toggle();
		}
		_ptm3pf = 0;
	}
}