#include "MyI2C.h"
#include "Interface.hpp"
#include "stdlib.h"




void MyI2C_Init(struct MyI2C* i2c, GPIO_TypeDef *SCL_port, GPIO_TypeDef *SDA_port, uint16_t SCL_pin, uint16_t SDA_pin){
    i2c->SCL_port = SCL_port;
    i2c->SDA_port = SDA_port;
    i2c->SCL_pin = SCL_pin;
    i2c->SDA_pin = SDA_pin;
    i2c->refcount = 1;

    ENABLE_GPIO_CLOCK(SCL_port);
    ENABLE_GPIO_CLOCK(SDA_port);
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = SCL_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(SCL_port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = SDA_pin;
    HAL_GPIO_Init(SDA_port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(SCL_port, SCL_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SDA_port, SDA_pin, GPIO_PIN_SET);
}


void MyI2C_Start(struct MyI2C* i2c){
    HAL_GPIO_WritePin(i2c->SDA_port, i2c->SDA_pin, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(i2c->SCL_port, i2c->SCL_pin, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(i2c->SDA_port,i2c->SDA_pin, GPIO_PIN_RESET);
    delay_us(10);
    HAL_GPIO_WritePin(i2c->SCL_port, i2c->SCL_pin, GPIO_PIN_RESET);
}


void MyI2C_Stop(struct MyI2C* i2c){
    HAL_GPIO_WritePin(i2c->SDA_port,i2c->SDA_pin, GPIO_PIN_RESET);
    delay_us(10);
    HAL_GPIO_WritePin(i2c->SCL_port, i2c->SCL_pin, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(i2c->SDA_port, i2c->SDA_pin, GPIO_PIN_SET);
    delay_us(10);
}


void MyI2C_SendByte(struct MyI2C* i2c,uint8_t data){
    uint8_t i = 0;
    for(i=0;i<8;i++){
        if((data&0x80)>>7){
            HAL_GPIO_WritePin(i2c->SDA_port, i2c->SDA_pin, GPIO_PIN_SET);
        }
        else{
            HAL_GPIO_WritePin(i2c->SDA_port,i2c->SDA_pin, GPIO_PIN_RESET);
        }
        delay_us(10);
        HAL_GPIO_WritePin(i2c->SCL_port, i2c->SCL_pin, GPIO_PIN_SET);
        delay_us(10);
        HAL_GPIO_WritePin(i2c->SCL_port, i2c->SCL_pin, GPIO_PIN_RESET);
        delay_us(10);
        data<<=1;
    }
}

uint8_t MyI2C_ReceiveByte(struct MyI2C* i2c){
    uint8_t Byte = 0x00;
    HAL_GPIO_WritePin(i2c->SDA_port, i2c->SDA_pin, GPIO_PIN_SET);
    delay_us(10);
    for(uint8_t i=0; i<8; i++){
        HAL_GPIO_WritePin(i2c->SCL_port, i2c->SCL_pin, GPIO_PIN_SET);
        delay_us(10);
        if(HAL_GPIO_ReadPin(i2c->SDA_port, i2c->SDA_pin) == GPIO_PIN_SET){
            Byte |= 0x01;
        }
        if(i<7){
            Byte<<=1;
        }
        HAL_GPIO_WritePin(i2c->SCL_port, i2c->SCL_pin, GPIO_PIN_RESET);
        delay_us(10);
    }
    return Byte;
}


void MyI2C_SendAck(struct MyI2C* i2c,uint8_t ack){
    if(ack){
        HAL_GPIO_WritePin(i2c->SDA_port, i2c->SDA_pin, GPIO_PIN_SET);
    }
    else{
        HAL_GPIO_WritePin(i2c->SDA_port, i2c->SDA_pin, GPIO_PIN_RESET);
    }
    delay_us(10);
    HAL_GPIO_WritePin(i2c->SCL_port, i2c->SCL_pin, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(i2c->SCL_port, i2c->SCL_pin, GPIO_PIN_RESET);
    delay_us(10);
}

uint8_t MyI2C_ReceiveAck(struct MyI2C* i2c){
    uint8_t ack;
    HAL_GPIO_WritePin(i2c->SDA_port, i2c->SDA_pin, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(i2c->SCL_port, i2c->SCL_pin, GPIO_PIN_SET);
    delay_us(10);
    if(HAL_GPIO_ReadPin(i2c->SDA_port, i2c->SDA_pin)==GPIO_PIN_SET){
        ack=1;
    }
    else{
        ack=0;
    }
    HAL_GPIO_WritePin(i2c->SCL_port, i2c->SCL_pin, GPIO_PIN_RESET);
    delay_us(10);
    return ack;
}

void MyI2C_DeInit(struct MyI2C *i2c){
    if(!i2c) return;
    if(i2c->refcount==1){
        free(i2c);
        i2c = NULL;
    }
}