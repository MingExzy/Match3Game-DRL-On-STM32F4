#include "RandomGenerate.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include <stdlib.h>

extern RNG_HandleTypeDef hrng;

void Myrandom_generate(uint8_t *num, uint8_t min, uint8_t max){
    uint32_t random_range = max - min + 1;
    HAL_StatusTypeDef status;
    uint32_t random_number;
    status = HAL_RNG_GenerateRandomNumber(&hrng, &random_number);
    if (status == HAL_OK) {
        *num = (random_number % random_range) + min;
    } else {
        *num = (rand() % random_range) + min; // Fallback to standard rand() if HAL RNG fails
    }
}
