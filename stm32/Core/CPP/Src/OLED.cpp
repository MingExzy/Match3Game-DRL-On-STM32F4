#include "OLED.hpp"
#include "Interface.hpp"
#include "stm32f4xx_hal.h"
#include "OLED_Font.h"
#include <stdint.h>



OLED::OLED(enum I2C_config type,void* config):_type(type){
    if(type == I2C_SOFTWARE){
        _myi2c = (struct MyI2C*)config; 
    }
    else if(type == I2C_HARDWARE){
        _hi2c = (I2C_HandleTypeDef*)config;
    }
}


OLED::~OLED(){
    if(_type == I2C_SOFTWARE){
        _myi2c->refcount--;
    }
    else if(_type == I2C_HARDWARE){
        _hi2c = NULL;
    }
};

        
void OLED::Init(){
    int i,j;
    for(i = 0;i<1000;i++){
        for(j=0;j<100;j++);
    }
    Write_Command(0xAE);   //关闭显示
    Write_Command(0xD5);   //设置显示时钟分频比/振荡器频率
    Write_Command(0x80);

    Write_Command(0xA8);   //设置多路复用率
    Write_Command(0x3F);

    Write_Command(0xD3);   //设置显示偏移
    Write_Command(0x00);

    Write_Command(0x40);	//设置起始行
    Write_Command(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	Write_Command(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	Write_Command(0xDA);	//设置COM引脚硬件配置
	Write_Command(0x12);
	
	Write_Command(0x81);	//设置对比度控制
	Write_Command(0xCF);

	Write_Command(0xD9);	//设置预充电周期
	Write_Command(0xF1);

	Write_Command(0xDB);	//设置VCOMH取消选择级别
	Write_Command(0x30);

	Write_Command(0xA4);	//设置整个显示打开/关闭

	Write_Command(0xA6);	//设置正常/倒转显示

	Write_Command(0x8D);	//设置充电泵
	Write_Command(0x14);

	Write_Command(0xAF);	//开启显示
		
	Clear();	
};

void OLED::Clear(){
    uint32_t i,j;
    for(i=0;i<8;i++){
        Set_Cursor(i, 0);
        for(j=0;j<128;j++){
            Write_Data(0x00);
        }
    }
}

void OLED::ShowChar(uint8_t y,uint8_t x, char Char){
    Set_Cursor((y-1)*2,  (x-1)*8); //这一步有点不太理解
    for(int i=0;i<8;i++){
        Write_Data(OLED_F8x16[Char - ' '][i]);
    }
    Set_Cursor((y-1)*2+1, (x-1)*8);
    for(int i=0;i<8;i++){
        Write_Data(OLED_F8x16[Char - ' '][i+8]);
    }
};

void OLED::ShowString(uint8_t y, uint8_t x, char* str){
    for(int i=0;str[i]!='\0';i++){
        ShowChar(y, x+i, str[i]);
    }
};

void OLED::ShowNumber(uint8_t y, uint8_t x, uint32_t num, uint8_t length){
    for(int i=0; i<length; i++){
        ShowChar(y, x+i, num/pow(10, length-i-1)% 10 +'0');
    }
};

void OLED::ShowSignedNumber(uint8_t y,uint8_t x, int32_t num, uint8_t length){
    uint32_t number1;
    if(num >= 0){
        ShowChar( y,  x,  '+');
        number1 = num;
    }
    else{
        ShowChar( y,  x,  '-');
        number1 = -num;
    }
    ShowNumber( y,  x+1, number1,  length);
};

void OLED::ShowHexNumber(uint8_t y , uint8_t x, uint32_t num, uint8_t length){
    uint8_t num_char;
    for(int i=0; i < length; i++){

        num_char = num/pow(16, length-i-1)%16;
        if(num_char<10){
            ShowChar( y,  x+i, num_char +'0');
        }
        else{
            ShowChar( y,  x+i, num_char-10+'A');
        }
    }
}

void OLED::ShowBinNumber(uint8_t y, uint8_t x, uint32_t num, uint8_t length){
    for(int i=0;i<length;i++){
        ShowChar( y,  x+i, num/pow(2, length-i-1)%2 +'0');
    }
}

extern "C"{
    OLED* OLED_Create(enum I2C_config type, void* config){
        OLED* oled = new OLED(type, config);
        return oled;
    };

    void OLED_Destroy(OLED* oled){
        delete oled;
    };

    void OLED_Init(OLED* oled){
        oled->Init();
    };

    void OLED_Clear(OLED* oled){
        oled->Clear();
    };
    
    void OLED_ShowChar(OLED* oled, uint8_t y, uint8_t x, char Char){
        oled->ShowChar(y, x, Char);
    };

    void OLED_ShowString(OLED* oled, uint8_t y, uint8_t x ,char* str){
        oled->ShowString(y, x, str);
    };

    void OLED_ShowNumber(OLED* oled, uint8_t y, uint8_t x, uint32_t num, uint8_t length){
        oled->ShowNumber(y, x,  num,  length);
    };

    void OLED_ShowSignedNumber(OLED* oled,uint8_t y, uint8_t x, int32_t num, uint8_t length){
        oled->ShowSignedNumber(y, x, num, length);
    };

    void OLED_ShowHexNumber(OLED* oled, uint8_t y, uint8_t x, uint32_t num, uint8_t length){
        oled->ShowHexNumber(y, x, num, length);
    };

    void OLED_ShowBinNumber(OLED* oled, uint8_t y, uint8_t x, uint32_t num, uint8_t length){
        oled->ShowBinNumber(y, x, num, length);
    };
}