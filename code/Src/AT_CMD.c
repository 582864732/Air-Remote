#include"AT_CMD.h"
#include"uart.h"
#include"bsp_gpio.h"
#include"oled.h"
#include"delay.h"
#include"string.h"

#define AT_SEND(string) UART_SendString(string)
#define AT_END() UART_SendString("\r\n")

ESP_StateStruct ESP_State;
StationTypeStruct StationType;
ESP_ClientStruct ESP_Client;

uint8 rx_data[128];
uint8 counter = 0;
uint8 function_receive_flag = 0;
uint8 receive_flag = 0;


#define rx_dataCompare(str,offset) (strncmp(rx_data+offset,str,strlen(str))==0)
#define GotoNext(i) while(rx_data[i]==0x0D||rx_data[i]==0x0A) i++;
// extern uint8 rx_data;
// extern uint8 counter;
void ESP_Init(void)
{
	// strcpy(AT_State.wifi_ssid,"WIFI");
	// strcpy(AT_State.wifi_password,"wifiwifi");
	// AT_State.server_port = 333;
	// AT_State.work_mode = AT_STATION_MODE;
	AT_SEND("ATE0");
	AT_END();
	delay_ms(1000);

	AT_SEND("AT");
	AT_END();
	AT_ReceiveOnce(100);
	uint8 i=0;
	// oled_show_string(0,1,"OK",0);
	if(ESP_Compare("OK",&i)==OK){
		led1_on();
	}
	delay_ms(100);

	AT_SEND("AT+CWMODE=2");
	AT_END();

	AT_SEND("AT+CIPAP_DEF=\"192.168.1.112\"");
	AT_END();
	//create server
	while(1){
		uint8 i=0;
		AT_SEND("AT+CIPMUX=1");
		AT_END();
		AT_Receive(500);
		if(ESP_Compare("OK",&i)!=OK) led1_toggle();
		AT_SEND("AT+CIPSERVER=1");
		AT_END();
		AT_Receive(500);
		if(ESP_Compare("OK",&i)!=OK) led1_toggle();
		else break;
	}
}

uint8 ESP_Disconnect(void)
{
	ESP_State.is_wifi_connected = 0;
	AT_SEND("AT+CWQAP");
	AT_END();
	delay_ms(10);
	return OK;
}

// uint8 ESP_SetMode(uint8 mode)
// {
// 	switch(mode){
// 		case AT_STATION_MODE:AT_SEND("AT+CWMODE=1");break;
// 		case AT_SOFTAP_MODE:AT_SEND("AT+CWMODE=2");break;
// 		case AT_SOFTAP_STATION_MODE:AT_SEND("AT+CWMODE=3");break;
// 		default:break;
// 	}
// 	AT_END();
// 	AT_ReceiveOnce(10);
// 	uint8 i=0;
// 	GotoNext(i);
// 	if(rx_dataCompare("OK",i)) return OK;
// 	else return ERROR;
// }

// uint8 ESP_SetupServer(void)
// {
// 	AT_SEND("AT+CWMODE=2");
// 	AT_END();
// 	delay_ms(1);
// 	AT_SEND("AT+CIPMUX=1");
// 	AT_END();
// 	AT_Receive(100);
// 	uint8 i=0;
// 	oled_show_string(0,1,rx_data,0);
// 	if(ESP_Compare("OK",&i)==OK) led1_on();
// 	delay_ms(1);
// 	delay_ms(1);
// 	AT_SEND("AT+CIPSERVER=1");
// 	AT_END();
// }

// uint8 ESP_ConnectWIFI(void)
// {
// 	ESP_Disconnect();
// 	uint8 str[50] = "AT+CWJAP_CUR=\"";
// 	uint8 i=0;
// 	strncat(str,StationType.ssid,strlen(StationType.ssid));
// 	strcat(str,"\",\"");
// 	strncat(str,StationType.password,strlen(StationType.password));
// 	strcat(str,"\"");
// 	AT_SEND(str);
// 	AT_END();
// 	if(AT_ReceiveOnce(100)==OK){
// 		// oled_show_string(0,0,rx_data,0);
// 		return ERROR;
// 	}
// 	if(AT_Receive(10000)==TIME_OUT) return TIME_OUT;
// 	i=0;
// 	GotoNext(i);
// 	// oled_show_string(0,5,rx_data+i,0);
// 	if(strncmp(&rx_data[i],"WIFI CONNECTED",14)==0){
// 		ESP_State.is_wifi_connected = 1;
// 		return OK;
// 	}
// 	else return CONNECT_ERROR;
// }

uint8 AT_ReceiveOnce(uint16 wait)
{
	while(receive_flag) ;
	function_receive_flag = 1;
	counter = 0;
	//RX_ISR_ENABLE();
	uint16 temp = 0;
	while (counter==0){
		temp++;
		if(temp==wait) {
			rx_data[0] = 0;
			return TIME_OUT;
		}
		delay_ms(1);
	}
	delay_ms(500);
	// led1_on();
	// delay_ms(wait);
	// RX_ISR_DISABLE();
	if(counter==0) rx_data[0] = 0;
	else rx_data[--counter] = 0;
	counter = 0;
	function_receive_flag = 0;
	return OK;
}

uint8 ESP_Send(uint8*data,uint8 length){
	static uint8 tx_header[3] = {0xAA,0xBB,0xCC};
	AT_SEND("AT+CIPSTATUS");
	AT_END();
	AT_Receive(500);
	uint8 i = 0;

	while(ESP_Compare("+CIPSTATUS:",&i)==OK){
		AT_SEND("AT+CIPSEND=");
		// i++;
		// oled_show_char(0,8,rx_data[i]);
		UART_SendByte(rx_data[i]);
		AT_SEND(",");
		AT_SEND(toString(length+3));
		AT_END();
		delay_ms(1);
		UART_SendByte(tx_header[0]);
		UART_SendByte(tx_header[1]);
		UART_SendByte(tx_header[2]);
		// UART_SendStringL(data,length);
		uint8 n=0;
		for(;n<length;n++){
			UART_SendByte(data[n]);

		}

	}
}

uint8 ESP_OK(void)
{
	AT_SEND("AT");
	AT_END();
	AT_Receive(100);
	uint8 i=0;
	return ESP_Compare("OK",&i);
}

uint8 ESP_Compare(uint8* str,uint8 *offset){
	uint8 i=0;
	uint8 len = strlen(str);
	while( rx_data[*offset]!=0){
		if(rx_data[*offset]==str[i]){
			i++;
			if(i==len){
				(*offset)++;
				return OK;
			}
		}else{
			i=0;
		}
		(*offset)++;
	}
	return ERROR;
}

uint8 AT_Receive(uint16 wait)
{
	while(receive_flag) ;
	function_receive_flag = 1;
	counter = 0;
	delay_ms(wait);
	if(counter==0) rx_data[0] = 0;
	else rx_data[--counter] = 0;
	counter = 0;
	function_receive_flag = 0;
	return OK;
}
