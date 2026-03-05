#ifndef _MyI2C_H
#define _MyI2C_H
#include "stm32f4xx_hal.h"
#include <stdint.h>



#ifdef __cplusplus
extern "C"{
#endif

struct MyI2C{
    GPIO_TypeDef* SCL_port;
    GPIO_TypeDef* SDA_port;
    uint16_t SCL_pin;
    uint16_t SDA_pin;
    uint8_t refcount;
};

void MyI2C_Init(struct MyI2C* i2c, GPIO_TypeDef* SCL_port,GPIO_TypeDef* SDA_port, uint16_t SCL_pin, uint16_t SDA_pin);
void MyI2C_Start(struct MyI2C* i2c);
void MyI2C_Stop(struct MyI2C* i2c);
void MyI2C_SendByte(struct MyI2C* i2c,uint8_t data);
uint8_t MyI2C_ReceiveByte(struct MyI2C* i2c);
void MyI2C_SendAck(struct MyI2C* i2c,uint8_t ack);
uint8_t MyI2C_ReceiveAck(struct MyI2C* i2c);
void MyI2C_DeInit(struct MyI2C *i2c);

#ifdef __cplusplus
}
#endif

#endif