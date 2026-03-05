#ifndef CONNECT_INTERFACE_HPP
#define CONNECT_INTERFACE_HPP
#include "stm32f4xx_hal.h"
#include "MyI2C.h"
#include "MySPI.h"



#ifdef __cplusplus
extern "C"{
    #endif

enum I2C_config{I2C_HARDWARE,I2C_SOFTWARE};
enum SPI_config{SPI_HARDWARE,SPI_SOFTWARE};




#ifdef __cplusplus
}
#endif

#endif