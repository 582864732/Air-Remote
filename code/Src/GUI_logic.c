#include"GUI.h"
#include"GUI_Logic.h"
#include"AT_CMD.h"
#include"bsp_gpio.h"
#include"delay.h"

extern ESP_StateStruct ESP_State;
extern uint8 AT_Command;
extern WindowStructure *CurrentWindow;

//main form
WindowStructure MainSettingWindow;

ButtonStructure WIFI_Setting_Button;

//WIFI form
WindowStructure WIFI_SettingWindow;

LabelStructure WIFI_Label;

ButtonStructure WorkAsAP_OrStationButton;
ButtonStructure ConnectOrDisconnectButton;

#define setPTM3P(value) _ptm3rpl = value&0xFF;_ptm3rph=(value>>8);

uint8 connecting_flag = 0;



void mainSettingFormInit(void)
{
    WIFI_Setting_Button.x = 0;
    WIFI_Setting_Button.y = 0;
    WIFI_Setting_Button.text = "WIFI Setting";
    WIFI_Setting_Button.next_button = NULL;

    MainSettingWindow.ButtonList = &WIFI_Setting_Button;
    MainSettingWindow.CL_List = NULL;
    MainSettingWindow.CurrentButton = &WIFI_Setting_Button;
    MainSettingWindow.TextList = NULL;
}

void WIFI_SettingButtonCallback(void){
    CurrentWindow = &WIFI_SettingWindow;
    WIFI_SettingWindow.CurrentButton = &WorkAsAP_OrStationButton;
    addChangeListS(&WIFI_Setting_Button);
}

void WIFI_SettingFormInit(void)
{
    WIFI_Label.x = 0;
    WIFI_Label.y = 0;
    WIFI_Label.text = "Station";
    WIFI_Label.reverse = 0;
    WIFI_Label.next_text = NULL;

    WorkAsAP_OrStationButton.x = 0;
    WorkAsAP_OrStationButton.y = 1;
    WorkAsAP_OrStationButton.text = "Work As AP";   
    WorkAsAP_OrStationButton.next_button = &ConnectOrDisconnectButton;

    ConnectOrDisconnectButton.x = 0;
    ConnectOrDisconnectButton.y = 2;
    ConnectOrDisconnectButton.text = "Connect to WIFI";
    ConnectOrDisconnectButton.next_button = NULL;

    WIFI_SettingWindow.ButtonList = &WorkAsAP_OrStationButton;
    WIFI_SettingWindow.CL_List = NULL;
    WIFI_SettingWindow.CurrentButton = &WorkAsAP_OrStationButton;
    WIFI_SettingWindow.TextList = &WIFI_Label;
}

void  WorkAsAP_OrStationButtonCallback(void)
{
    // CurrentWindow = &WIFI_SettingWindow;
    // led2_on();
    if(ESP_State.current_mode == AT_STATION_MODE){
        WorkAsAP_OrStationButton.text="Work As Station";
        WIFI_Label.text = "Station";
        ESP_State.current_mode = AT_SOFTAP_MODE;
        
    }else if(ESP_State.current_mode == AT_SOFTAP_MODE){
        WorkAsAP_OrStationButton.text="Work As AP";
        ESP_State.current_mode = AT_STATION_MODE;
        WIFI_Label.text = "AP";
    }
    addChangeListS(&WorkAsAP_OrStationButton);
    addChangeListS(&WIFI_Label);
}

void ConnectOrDisconnectButtonCallback(void)
{
    if(ESP_State.is_wifi_connected==0){
        // addChangeListS(&ConnectOrDisconnectButton);
        // addChangeListS(&WIFI_Label);
        // addChangeListS(&WorkAsAP_OrStationButton);
        // GUI_Clear();
        // oled_show_string(24,4,"connecting",0);
        // connecting_flag = 1;
        // setPTM3P(357142);
        // _pt3on = 1;
        // uint8 state = ESP_ConnectWIFI();
        // _pt3on = 0;
        // oled_clear_line(0,4,16);
        // if(state==OK) oled_show_string(0,4,"connected",0);
        // else oled_show_string(0,4,"connect error",0);
        // delay_ms(2000);
        // oled_clear_line(0,4,16);
        // connecting_flag = 0;
    }else{
        addChangeListS(&ConnectOrDisconnectButton);
        ConnectOrDisconnectButton.text = "Connect WIFI";
    }
}

void ButtonCallFunction(ButtonStructure *button)
{
	if(CurrentWindow==&MainSettingWindow){
        if(button == &WIFI_Setting_Button );
            WIFI_SettingButtonCallback();
    }else if(CurrentWindow==&WIFI_SettingWindow){
        if(button == &WorkAsAP_OrStationButton)
            WorkAsAP_OrStationButtonCallback();
        else if(button == &ConnectOrDisconnectButton)
            ConnectOrDisconnectButtonCallback();
    }
}

void WindowCallback(uint8 event)
{

}

void CustomizedLayerCallFunction(CustomizedLayerStructure *cl)
{
	
}

inline void GUI_Init(void)
{
	CurrentWindow->TextList = NULL;
	CurrentWindow->ButtonList = NULL;
	CurrentWindow->CL_List = NULL;
	CurrentWindow->CurrentButton = NULL;

	// setCurrentWindow(&ControllerWindow);
	mainSettingFormInit();
	WIFI_SettingFormInit();
    // controllerFormInit();
}