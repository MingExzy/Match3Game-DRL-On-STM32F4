#ifndef _MySPI_H_
#define _MySPI_H_
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C"{
#endif
    struct MySPI{
        GPIO_TypeDef* SCK_port;
        GPIO_TypeDef* MISO_port;
        GPIO_TypeDef* MOSI_port;
        uint16_t sck_pin;
        uint16_t miso_pin;
        uint16_t mosi_pin;
        uint8_t refcount;
        bool is_working;
    };

    void MySPI_Init(struct MySPI* spi, GPIO_TypeDef* SCK_port,
                    GPIO_TypeDef* MISO_port,GPIO_TypeDef* MOSI_port, 
                    uint16_t SCK_pin, uint16_t MISO_pin, 
                    uint16_t MOSI_pin);
    void MySPI_DeInit(struct MySPI *spi);
    void MySPI_Start(struct MySPI* spi, GPIO_TypeDef*SS_port, uint16_t ss_pin);
    void MySPI_Stop(struct MySPI* spi, GPIO_TypeDef*SS_port, uint16_t ss_pin);
    void MySPI_WriteSCK(struct MySPI* spi, uint8_t value);
    uint8_t MySPI_WriteReadByte(struct MySPI* spi, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif