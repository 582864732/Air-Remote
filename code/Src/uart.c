#include"uart.h"
#include"oled.h"
#include"bsp_gpio.h"
#include"AT_CMD.h"

uint8 internet_data[6];
uint8 internet_length = 3;
uint8 internet_cmd;

//只使用UART0作为串口，更改寄存器改变配置
void UART_Init(void)
{
	//设置PA6,PA7为串口
	_pas1 &= 0x0F;
	_pas1 |= 0xF0;
	
	//设置RX0输入源引脚为PA6
	_rx0ps = 0;
	
	/*
	_u0cr1 ：
	bit 7 UART使能位
	bit 6 选择发送数据位数 0:8位发送 
	bit 5 奇偶校验使能位
	bit 4 奇偶校验选择位 0:偶校验
	bit 3停止位长度选择 0:一位停止位
	bit 2 暂停字发送控制位 0:不发送
	bit 1 RX第9位
	bit 0 TX第9位
	
	_u0cr2:
	bit 7 UART发送使能位
	bit 6 接收使能位
	bit 5 波特率高低速选择位 0:低速
	bit 4 地址检验使能位
	bit 3 RX脚下降沿唤醒UART功能使能位
	bit 2 接收中断使能
	bit 1 发送器空闲中断使能
	bit 0 发送寄存器为空使能位
	
	波特率计算：
	低速：
	f/(64(_brg0+1))
	高速：
	f/(16(_brg0+1))
	*/
	_u0cr1 = 0b10000000;
	_u0cr2 = 0b11001100;
	
	// _brg0 = 5;
	_brg0 = 17; // 11059200/(64*18) = 9600
	
	//在_u0cr1中
	_uarten0 = 1;
	_txen0 = 1;
	_rxen0 = 1;
	
	//中断使能，并置零中断标志位
	_mf5e = 1;
	_mf5f = 0;
	_ur0f = 0;
	_ur0e = 1;
	_emi = 1;

	_pgs0 = 0x0F;
	_rx2ps = 1;

	_u2cr1 = 0b10000000;
	_u2cr2 = 0b11001100;

	_brg2 = 17;

	_uarten2 = 1;
	_txen2 = 1;
	_rxen2 = 1;

	_ur2f = 0;
	_ur2e = 1;
}

void UART2_SendByte(uint8 byte)
{
	_ur2e = 0;
	while(_txif2==0);
	_txr_rxr2 = byte;
	_ur2e = 1;
}

void controllerSend(uint8 data[3]){
	UART2_SendByte(0xA1);
	UART2_SendByte(0xF1);
	UART2_SendByte(data[0]);
	UART2_SendByte(data[1]);
	UART2_SendByte(data[2]);
}

void UART_SendByte(uint8 byte)
{
	_ur0e = 0;
	while(_txif0 == 0);
	_txr_rxr0 = byte;
	_ur0e = 1;
}

void UART_SendStringL(uint8 *string,uint32 length)
{
	uint8 i=0;
	for(;i<length;i++){
		UART_SendByte(string[i]);
	}
}

void UART_SendString(uint8 *string)
{
	uint8 i=0;
	while(string[i]!=0){
		UART_SendByte(string[i++]);
	}
}

void UART_SendNum(uint32 num)
{
	uint8 ch[10];
	uint8 i=9;
	do{
		//UART_SendByte(num%10+0x30);
		ch[i--] = num%10+0x30;
		num=num/10;
	}while(num!=0);
	UART_SendStringL(ch+i+1,9-i);
}

void UART_SendStrings(uint8 **strs,uint8 length)
{
	uint8 i=0;
	for(;i<length;i++){
		UART_SendString(strs[i]);
	}
}


extern uint8 rx_data[128];
extern uint8 counter;
extern uint8 function_receive_flag;
extern uint8 receive_flag;
extern uint8 tx_data[4];
extern uint8 esp_rx_flag;
extern uint8 controller_data[3];
extern uint8 controller_rx_flag;
uint8 controller_counter = 0;

void __attribute((interrupt(0x3c))) UART_Receive(void)
{
	static uint8 sw_mode = 0;
	static uint8 temp = 0;
	GCC_NOP();
	// led1_on();

	if(_ur0f==1)
	{
		if(_perr0==1||_oerr0==1||_nf0==1||_ferr0==1)
		{
			asm("lmov a,___txr_rxr0");	//read RXR0 register to clear RXIF0 bit
		}else{
			while(_rxif0==0);
			temp = _txr_rxr0;
			if(function_receive_flag==0){
				led2_toggle();
				static uint8 r[50];
				static c = 0;
				r[c++] = temp;
				if(c==30){
					led1_toggle();
					c=0;
				}
				switch (sw_mode){
				case 0:
					if(temp==0xCC){
						sw_mode=3;
					}
					else {
						sw_mode=0;
						receive_flag=0;
					}
					break;
				case 3:
					tx_data[counter] = temp;
					counter++;
					if(counter==internet_length){
						// tx_data[3] = ~tx_data[2];
						receive_flag=0;
						esp_rx_flag = 1;
						sw_mode = 0;
						counter=0;
					}
					break;
				default:
					sw_mode=0;
					internet_length=0;
					internet_cmd = 0;
					break;
				}
			}else{
				if(counter==128)
				{
					_ur0f = 0;
					return;
				}
				rx_data[counter] = _txr_rxr0;
				counter++;
				// if(counter==5) led2_on();
			}
		}
		_ur0f=0;
	}
	else if(_ur2f == 1){
		if(_perr2==1||_oerr2==1||_nf2==1||_ferr2==1)
		{
			asm("lmov a,___txr_rxr2");	//read RXR0 register to clear RXIF0 bit
		}else{
			while(_rxif2==0);
			if(controller_counter==0){
				_pt2on = 1;
			}
			controller_data[controller_counter++] = _txr_rxr2;
			if(controller_counter==3){
				controller_counter = 0;
				controller_rx_flag = 1;
				_pt2on=0;
			}
		}
		_ur2f=0;
	}
}

void __attribute((interrupt(0x20))) PTM2_Receive(void)
{
	controller_counter=0;
	_ptm2pf=0;
	_pt2on = 0;
}