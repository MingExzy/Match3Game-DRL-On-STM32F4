 #ifndef INTERFACE_HPP
 #define INTERFACE_HPP
#include <stdbool.h>
#include <stdint.h>
#include <sys/_types.h>
#include "MyI2C.h"
#include "MySPI.h"
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "Connect_Interface.h"


#define ENABLE_GPIO_CLOCK(port) \
    do { \
        if(port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE(); \
        else if(port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE(); \
        else if(port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE(); \
        else if(port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE(); \
        else if(port == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE(); \
    } while(0)

typedef enum: uint32_t {
    OK = 1,
    IFAILED = 0,
    TIMEOUT = 2,
}ErrorCode;


#ifdef __cplusplus
extern "C"{
    #endif

void delay_us(uint32_t us);


typedef struct LED LED;
LED* LED_Create(GPIO_TypeDef* port, uint16_t GPIO_Pin);
void LED_Destroy(LED* led);
void LED_Toggle(LED* led);

typedef struct Key Key;
Key* Key_Create(GPIO_TypeDef* port, uint16_t pin);
void Key_Destroy(Key* key);
bool Key_IsPressed(Key* key);

typedef struct OLED OLED;
OLED* OLED_Create(enum I2C_config type, void* config);
void OLED_Destroy(OLED* oled);
void OLED_Init(OLED* oled);
void OLED_Clear(OLED* oled);
void OLED_ShowChar(OLED* oled, uint8_t y, uint8_t x, char Char);
void OLED_ShowString(OLED* oled, uint8_t y, uint8_t x ,char* str);
void OLED_ShowNumber(OLED* oled, uint8_t y, uint8_t x, uint32_t num, uint8_t length);
void OLED_ShowSignedNumber(OLED* oled,uint8_t y, uint8_t x, int32_t num, uint8_t length);
void OLED_ShowHexNumber(OLED* oled, uint8_t y, uint8_t x, uint32_t num, uint8_t length);
void OLED_ShowBinNumber(OLED* oled, uint8_t y, uint8_t x, uint32_t num, uint8_t length);

typedef struct Match3Env Match3Env;
Match3Env* Match3Env_Create(void);
void Match3Env_Destroy(Match3Env* env);
void Match3Env_Reset(Match3Env* env);
void Match3Env_Step(Match3Env* env, uint8_t action,int32_t* reward, bool* done, char* info);
void Match3Env_GetBoard(Match3Env* env, int8_t *board_);


#ifdef __cplusplus
}
#endif
#endif