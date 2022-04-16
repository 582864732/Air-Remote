#include"EEPROM.h"
#include"bsp_gpio.h"
#include"OLED.h"

uint8 EEPROM_ReadBytes(uint8 *get_data,uint16 adress,uint8 length)
{
    uint8 i=0;
    for(;i<length;i++){
        get_data[i] = EEPROM_ReadByte(adress);
        adress++;
    }
    return OK;
}

uint8 EEPROM_WriteBytes(uint16 adress,uint8* data,uint8 length)
{
    uint8 i=0;
    for(;i<length;i++){
        EEPROM_WriteByte(adress,data[i]);
        adress++;
    }
    return OK;
}

void EEPROM_WriteByte(uint16 adress,uint8 data)
{
    _eeal = adress&0xFF;
    _eeah = (adress>>8)&0x03;
    _eed = data;

    _mp2l = 0x40;
    _mp2h = 0x01;
    _emi=0;
    _iar2 = 0x08;
    _iar2 = 0x04;
    while(_iar2&0x04);
    _iar2 &= 0xf7;
    _emi=1;
    _mp2h = 0;
    // asm("mov a,04h");
    // asm("mov [03h],a");
    // asm("mov a,01h");
    // asm("mov [04h],a");
    // asm("clr [10h].0");
    // asm("set [02h].3");//[02h]==iar1
    // asm("set [02h].2");
    // asm("set [10h].0");//emi
    // asm("back1:");
    // asm("sz [02h].2");
    // asm("jmp back1");
    // asm("clr [02h]");
    // asm("clr [04h]");
}

uint8 EEPROM_ReadByte(uint16 adress)
{
    uint8 data = 0;;

    _eeal = adress&0xFF;
    _eeah = (adress>>8)&0x03;

    _mp1l = 0x40;
    _mp1h = 0x01;
    _iar1 |= 0x02;
    _iar1 |= 0x01;
    while(_iar1&0x01); 
    data = _eed;
    _iar1 = 0xfd;
    return data;
    // asm("mov a,04h");
    // asm("mov [03h],a");
    // asm("mov a,01h");
    // asm("mov [04h],a");
    // asm("set [02h].1");
    // asm("set [02h].0");
    // asm("back2:");
    // asm("sz [02h].0");
    // asm("jmp back2");
    // asm("clr [02h]");
    // asm("clr [04h]");
    // // asm("mov a,[43h]");
    // asm("mov read_data,a");
    // return read_data;
}