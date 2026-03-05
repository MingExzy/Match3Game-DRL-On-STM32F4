 #ifndef _LED_HPP_
 #define _LED_HPP_
#include "stm32f4xx_hal.h"

class LED {
public:
    LED(GPIO_TypeDef* port, uint16_t GPIO_Pin);
    void toggle();
    ~LED();
private:
    GPIO_TypeDef* m_port;
    uint16_t m_GPIO_Pin;
};

#endif