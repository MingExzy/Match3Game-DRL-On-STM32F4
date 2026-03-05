#include "MyTimer.h"
#include "main.h"

#include <sys/_types.h>

void Timer_Base_Set(
    TIM_HandleTypeDef* htimx, 
    TIM_TypeDef *instance, 
    uint32_t prescaler, 
    uint32_t counter_mode, 
    uint32_t period, 
    uint32_t clock_div, 
    uint32_t auto_reload_preload){

        htimx->Instance = instance;
        htimx->Init.Prescaler = prescaler;
        htimx->Init.CounterMode = counter_mode;
        htimx->Init.Period = period;
        htimx->Init.ClockDivision =clock_div;
        htimx->Init.AutoReloadPreload = auto_reload_preload;

        if(HAL_TIM_Base_Init(htimx)!=HAL_OK){
            Error_Handler();
        };
    }

void Timer_OC_Config_Set(
    TIM_HandleTypeDef* htimx, 
    uint32_t oc_mode, 
    uint32_t oc_polarity, 
    uint32_t pulse,
    unsigned int Channel){

        TIM_OC_InitTypeDef TIM_OCInitStruct = {0};
        TIM_OCInitStruct.OCMode = oc_mode;
        TIM_OCInitStruct.OCPolarity = oc_polarity;
        TIM_OCInitStruct.Pulse = pulse;
        
        if(HAL_TIM_OC_Init(htimx)!=HAL_OK){
            Error_Handler();
        };

        if(HAL_TIM_OC_ConfigChannel(htimx, &TIM_OCInitStruct, Channel)!=HAL_OK){
            Error_Handler();
        }
    }


void Timer_ClockSource(
    TIM_HandleTypeDef* htimx, 
    uint32_t clock_source,
    uint32_t filter,
    uint32_t polarity,
    uint32_t prescaler){
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    sClockSourceConfig.ClockSource = clock_source;
    sClockSourceConfig.ClockFilter = filter;
    sClockSourceConfig.ClockPrescaler = prescaler;
    sClockSourceConfig.ClockPolarity = polarity;
    if(HAL_TIM_ConfigClockSource(htimx, &sClockSourceConfig)!=HAL_OK){
        Error_Handler();
    }
}

void Timer_PWM_Config_Set(
    TIM_HandleTypeDef* htimx, 
    uint32_t oc_mode,
    uint32_t oc_polarity,
    uint32_t pulse,
    unsigned int Channel){

        TIM_OC_InitTypeDef TIM_OCStruct = {0};
        TIM_OCStruct.OCMode = oc_mode;
        TIM_OCStruct.OCPolarity = oc_polarity;
        TIM_OCStruct.Pulse = pulse;

        if(HAL_TIM_PWM_Init(htimx)!=HAL_OK){
            Error_Handler();
        };

        if(HAL_TIM_PWM_ConfigChannel(htimx,&TIM_OCStruct,Channel)!=HAL_OK){
            Error_Handler();
        };
    }
