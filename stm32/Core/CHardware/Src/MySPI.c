#include "MySPI.h"

#include "Interface.hpp"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


void MySPI_Init(struct MySPI* spi, GPIO_TypeDef* SCK_port,
                GPIO_TypeDef* MISO_port,GPIO_TypeDef* MOSI_port, 
                uint16_t SCK_pin, uint16_t MISO_pin, uint16_t MOSI_pin)
                {
                    spi->SCK_port = SCK_port;
                    spi->MISO_port = MISO_port;
                    spi->MOSI_port = MOSI_port;
                    spi->sck_pin = SCK_pin;
                    spi->miso_pin = MISO_pin;
                    spi->mosi_pin = MOSI_pin;
                    spi->refcount = 1;
                    spi->is_working = false;

                    ENABLE_GPIO_CLOCK(SCK_port);
                    ENABLE_GPIO_CLOCK(MISO_port);
                    ENABLE_GPIO_CLOCK(MOSI_port);

                    GPIO_InitTypeDef GPIO_InitStruct = {0};
                    GPIO_InitStruct.Pin = SCK_pin;
                    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

                    HAL_GPIO_Init(SCK_port, &GPIO_InitStruct);
                    HAL_GPIO_WritePin(SCK_port, SCK_pin, GPIO_PIN_RESET);

                    GPIO_InitStruct.Pin = MOSI_pin;
                    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

                    HAL_GPIO_Init(MOSI_port, &GPIO_InitStruct);
                    HAL_GPIO_WritePin(MOSI_port, MOSI_pin, GPIO_PIN_RESET);

                    GPIO_InitStruct.Pin = MISO_pin;
                    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
                    GPIO_InitStruct.Pull = GPIO_PULLUP;

                    HAL_GPIO_Init(MISO_port, &GPIO_InitStruct);

}

void MySPI_DeInit(struct MySPI *spi){
    if(spi->refcount == 1){
        free(spi);
        spi = NULL;
    }
}



void MySPI_Start(struct MySPI* spi, GPIO_TypeDef*SS_port, uint16_t ss_pin){
    if(spi->is_working){
        return;
    }
    spi->is_working = true;
    HAL_GPIO_WritePin(SS_port, ss_pin, GPIO_PIN_RESET);
}

void MySPI_Stop(struct MySPI* spi, GPIO_TypeDef*SS_port, uint16_t ss_pin){
    if(!spi->is_working){
        return;
    }
    spi->is_working = false;
    HAL_GPIO_WritePin(SS_port, ss_pin, GPIO_PIN_SET);
}


void MySPI_WriteSCK(struct MySPI* spi, uint8_t value){
    if(value){
        HAL_GPIO_WritePin(spi->SCK_port, spi->sck_pin, GPIO_PIN_SET);
    }
    else{
        HAL_GPIO_WritePin(spi->SCK_port, spi->sck_pin, GPIO_PIN_RESET);
    }
}



uint8_t MySPI_WriteReadByte(struct MySPI* spi, uint8_t data){
    uint8_t Byte = 0x00;
    // for(uint8_t i=0; i<8; i++){
    //     if(data & (0x80>>i)){
    //         HAL_GPIO_WritePin(spi->MOSI_port, spi->mosi_pin, GPIO_PIN_SET);
    //     }
    //     else{
    //         HAL_GPIO_WritePin(spi->MOSI_port, spi->mosi_pin, GPIO_PIN_RESET);
    //     }
    //     MySPI_WriteSCK(spi, 1);
    //     if(HAL_GPIO_ReadPin(spi->MISO_port, spi->miso_pin)==GPIO_PIN_SET){
    //         Byte |= (0x80>>i);
    //     }
    //     MySPI_WriteSCK(spi, 0);
    // }



    for(uint8_t i=0; i<8;i++){
        if(data & 0x80){
            HAL_GPIO_WritePin(spi->MOSI_port, spi->mosi_pin, GPIO_PIN_SET);
        }
        else{
            HAL_GPIO_WritePin(spi->MOSI_port, spi->mosi_pin, GPIO_PIN_RESET);
        }
        MySPI_WriteSCK(spi, 1);
        if(HAL_GPIO_ReadPin(spi->MISO_port, spi->miso_pin)==GPIO_PIN_SET){
            Byte |= (0x80>>i);
        }
        MySPI_WriteSCK(spi, 0);
        data <<= 1;

    }

    return Byte;
}

