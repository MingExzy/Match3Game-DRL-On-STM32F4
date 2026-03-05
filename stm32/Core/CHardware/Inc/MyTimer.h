#ifndef _MYTIMER_H
#define _MYTIMER_H
#include "stm32f4xx_hal.h"
#include <stdint.h>


void Timer_Base_Set(
    TIM_HandleTypeDef* htimx,
    TIM_TypeDef* instance,
    uint32_t prescaler,
    uint32_t counter_mode,
    uint32_t period,
    uint32_t clock_div,
    uint32_t auto_reload_preload);

void Timer_OC_Config_Set(
    TIM_HandleTypeDef* htimx, 
    uint32_t oc_mode, 
    uint32_t oc_polarity, 
    uint32_t pulse,
    unsigned int Channel);

void Timer_ClockSource(
    TIM_HandleTypeDef* htimx, 
    uint32_t clock_source,
    uint32_t filter,
    uint32_t polarity,
    uint32_t prescaler);

void Timer_PWM_Config_Set(
    TIM_HandleTypeDef* htimx, 
    uint32_t oc_mode,
    uint32_t oc_polarity,
    uint32_t pulse,
    unsigned int Channel);

#endif