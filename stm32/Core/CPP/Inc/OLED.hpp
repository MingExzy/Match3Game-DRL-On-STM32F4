#ifndef _OLED_HPP_
#define _OLED_HPP_
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "MyI2C.h"
#include "Connect_Interface.h"


class OLED{
public:
    OLED(enum I2C_config type,void* config);
    void Init();
    void Clear();
    void ShowChar(uint8_t y, uint8_t x, char Char);
    void ShowNumber(uint8_t y,uint8_t x, uint32_t num, uint8_t length);
    void ShowString(uint8_t y, uint8_t x ,char* str);
    void ShowSignedNumber(uint8_t y,uint8_t x,int32_t num, uint8_t length);
    void ShowHexNumber(uint8_t y, uint8_t x, uint32_t num, uint8_t length);
    void ShowBinNumber(uint8_t y,uint8_t x, uint32_t num, uint8_t length);
    ~OLED();

private:
    enum I2C_config _type;
    I2C_HandleTypeDef* _hi2c = NULL;
    struct MyI2C* _myi2c = NULL;

    void I2C_SendByte(uint8_t byte){
        if(_type == I2C_SOFTWARE){
            MyI2C_SendByte(_myi2c, byte);
            HAL_GPIO_WritePin(_myi2c->SCL_port,_myi2c->SCL_pin,GPIO_PIN_SET); //额外的第九个时钟，不处理应答信号，这是OLED的宽容性，可以不处理应答信号
            HAL_GPIO_WritePin(_myi2c->SCL_port, _myi2c->SCL_pin, GPIO_PIN_RESET);
        }
        else if(_type == I2C_HARDWARE){
            HAL_I2C_Master_Transmit(_hi2c, 0x78, &byte, 1, 1000);
        }
    };

    void Write_Data(uint8_t data){
        if(_type == I2C_SOFTWARE){
            MyI2C_Start(_myi2c);
            I2C_SendByte(0x78);    //从机地址
            I2C_SendByte(0x40);    //写命令
            I2C_SendByte(data);
            MyI2C_Stop(_myi2c);
        }
        else if(_type == I2C_HARDWARE){
            uint8_t buffer[2] = {0x40, data};
            HAL_I2C_Master_Transmit(_hi2c, 0x78, buffer, 2, 1000);
        }
    
    };


    void Write_Command(uint8_t command){
        if(_type == I2C_SOFTWARE){
            MyI2C_Start(_myi2c);
            I2C_SendByte(0x78);
            I2C_SendByte(0x00);
            I2C_SendByte(command);
            MyI2C_Stop(_myi2c);
        }
        else if(_type == I2C_HARDWARE){
            uint8_t data[2] = {0x00, command};
            HAL_I2C_Master_Transmit(_hi2c, 0x78, data, 2, 1000);
        }
    };

    void Set_Cursor(uint8_t y, uint8_t x){
        Write_Command(0xB0|y); //设置y位置,OLED芯片的定位命令需要
        Write_Command(0x10|((x&0xF0)>>4)); //提取x位置信息的高位信息，因为芯片设计要求，
        // I2C要分别输入高部分和低部分；
        Write_Command(0x00|(x&0x0F));   //提取x轴信息的低位部分并传输
    };

    uint32_t pow(uint32_t x , uint32_t y){
        uint32_t result = 1;
        while(y){
            result = result*x;
            y--;
        }
        return result;
    };


};

#endif