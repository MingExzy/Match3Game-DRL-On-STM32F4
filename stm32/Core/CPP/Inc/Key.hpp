#ifndef _KEY_HPP_
#define _KEY_HPP_
#include "stm32f4xx_hal.h"

class Key{
public:
    Key(GPIO_TypeDef* port, uint16_t pin);
    bool IsPressed();
    ~Key();
private:
    GPIO_TypeDef* m_port;
    uint16_t m_Pin;
};

#endif