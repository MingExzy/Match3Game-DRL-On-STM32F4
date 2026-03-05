#include "Key.hpp"
#include "Interface.hpp"

Key::Key(GPIO_TypeDef* port, uint16_t pin) :m_port(port),m_Pin(pin){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    ENABLE_GPIO_CLOCK(port);
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
    
}

Key::~Key() {}


bool Key::IsPressed() {

    if (HAL_GPIO_ReadPin(m_port,m_Pin) == GPIO_PIN_RESET){
        HAL_Delay(20);
        while (HAL_GPIO_ReadPin(m_port,m_Pin) == GPIO_PIN_RESET) {};
        HAL_Delay(20);
        return true; 
    }
    return false;
}


// C jiekou

extern "C" Key* Key_Create(GPIO_TypeDef* port, uint16_t GPIO_Pin){
    Key* key = new Key(port,GPIO_Pin);
    return key;
}

extern "C" void Key_Destroy(Key* key){
    delete key;
}

extern "C" bool Key_IsPressed(Key *key){
    return key->IsPressed();
}

