#include "Interface.hpp"

extern "C"{

void delay_us(uint32_t us){
    static uint32_t fac_us = HAL_RCC_GetHCLKFreq()/1000000;
    uint32_t ticks = us * fac_us;
    uint32_t reload = SysTick->LOAD;
    uint32_t told = SysTick->VAL;
    uint32_t tnow, tcnt = 0;
    while(1){
        tnow = SysTick->VAL;
        if(tnow != told){
            if(tnow < told) tcnt += told - tnow;
            else tcnt += reload - tnow + told;
            told = tnow;
            if(tcnt >= ticks) break;
        }
    }

};

}
