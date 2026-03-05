#include "LED.hpp"
#include "Interface.hpp"

LED::LED(GPIO_TypeDef* port, uint16_t pin) :m_port(port),m_GPIO_Pin(pin){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    ENABLE_GPIO_CLOCK(port);
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
    
}

LED::~LED() {}


void LED::toggle() {
    HAL_GPIO_TogglePin(m_port,m_GPIO_Pin);
}


// C jiekou

extern "C" LED* LED_Create(GPIO_TypeDef* port, uint16_t GPIO_Pin){
    LED* led = new LED(port,GPIO_Pin);
    return led;
}

extern "C" void LED_Destroy(LED *led){
    delete led;
}

extern "C" void LED_Toggle(LED *led){
    led->toggle();
}

