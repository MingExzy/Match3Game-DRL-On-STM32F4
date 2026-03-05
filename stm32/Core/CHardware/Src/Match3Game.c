#include "Match3Game.h"
#include "Interface.hpp"
#include "stm32f4xx_hal.h"
#include <stdint.h>


void Match3Game_Render(Match3Env* env, OLED* oled, int8_t *board){
    
    OLED_ShowString(oled, 1, 9, "p:");
    OLED_ShowString(oled, 2, 9, "a:");
    OLED_ShowString(oled, 4, 9, "r:");
    
    for(uint8_t i = 0; i < 4; i++){
    uint8_t col = 1;
    for(uint8_t j = 0; j < 4; j++){
      if(board[i*4+j] == 1){
        OLED_ShowChar(oled, i+1, j+col, 'A');
      }
      else if(board[i*4+j] == 2){
        OLED_ShowChar(oled, i+1, j+col, 'B');
      }
      else if(board[i*4+j] == 3){
        OLED_ShowChar(oled, i+1, j+col, 'C');
      }
      if(j != 3){
        OLED_ShowChar(oled, i+1, j+col+1, '|');
        col++;
      }
      
    }
  }
}

void Match3Game_Swap(OLED* oled, uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2, int8_t *board){
    if(y1>4 || y1<1 || x1>4 || x1<1 || y2>4 || y2<1 || x2>4 || x2<1){
        return;
    }
    uint8_t x1_,y1_,x2_,y2_;
    XY_to_coordinates(x1, y1, &x1_, &y1_);
    XY_to_coordinates(x2, y2, &x2_, &y2_);
    OLED_ShowChar(oled, y1_, x1_, ' ');
    OLED_ShowChar(oled, y2_, x2_, ' ');
    HAL_Delay(500);
    if(board[(y1-1)*4+(x1-1)] == 1){
        OLED_ShowChar(oled, y1_, x1_, 'A');
    }
    else if(board[(y1-1)*4+(x1-1)] == 2){
        OLED_ShowChar(oled, y1_, x1_, 'B');
    }
    else if(board[(y1-1)*4+(x1-1)] == 3){
        OLED_ShowChar(oled, y1_, x1_, 'C');
    }

    if(board[(y2-1)*4+(x2-1)] == 1){
        OLED_ShowChar(oled, y2_, x2_, 'A');
    }
    else if(board[(y2-1)*4+(x2-1)] == 2){
        OLED_ShowChar(oled, y2_, x2_, 'B');
    }
    else if(board[(y2-1)*4+(x2-1)] == 3){
        OLED_ShowChar(oled, y2_, x2_, 'C');
    }

    HAL_Delay(500);
    OLED_ShowChar(oled, y1_, x1_, ' ');
    OLED_ShowChar(oled, y2_, x2_, ' ');
    HAL_Delay(500);

    if(board[(y1-1)*4+(x1-1)] == 1){
        OLED_ShowChar(oled, y2_, x2_, 'A');
    }
    else if(board[(y1-1)*4+(x1-1)] == 2){
        OLED_ShowChar(oled, y2_, x2_, 'B');
    }
    else if(board[(y1-1)*4+(x1-1)] == 3){
        OLED_ShowChar(oled, y2_, x2_, 'C');
    }

    if(board[(y2-1)*4+(x2-1)] == 1){
        OLED_ShowChar(oled, y1_, x1_, 'A');
    }
    else if(board[(y2-1)*4+(x2-1)] == 2){
        OLED_ShowChar(oled, y1_, x1_, 'B');
    }
    else if(board[(y2-1)*4+(x2-1)] == 3){
        OLED_ShowChar(oled, y1_, x1_, 'C');
    }
    HAL_Delay(500);
}

void Match3Game_Fresh(OLED* oled){
    for(uint8_t i = 1; i<=4; i++){
        for(uint8_t j = 1; j<=7; j++){
            OLED_ShowChar(oled, i, j, '*');
        }
    }
}


void Match3Game_Action_to_XY(uint8_t action, uint8_t* y1, uint8_t* x1, uint8_t* y2, uint8_t* x2){
    if(action<0 || action>= 2*4*(4-1)){
        //invalid action
        return;
    }
    if(action<4*(4-1)){
        *y1 = action / (4-1) + 1;
        *x1 = action % (4-1) + 1;
        *y2 = *y1;
        *x2 = *x1 + 1;
    }
    else{
        uint8_t action_ = action - 4*(4-1);
        *y1 = action_ / 4 + 1;
        *x1 = action_ % 4 + 1;
        *y2 = *y1 + 1;
        *x2 = *x1;
    }
}

int Match3Game_XY_to_Action(uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2){
    if(y1>4 || y1<1 || x1>4 || x1<1 || y2>4 || y2<1 || x2>4 || x2<1){
        //invalid action
        return -1;
    }
    uint8_t action;
    if(y1 == y2 && (x1 == x2 + 1 || x1 == x2 - 1)){
        action = (y1-1)*(4-1) + (x1-1 < x2-1 ? x1-1 : x2-1);
        return action;
    }
    else if(x1 == x2 && (y1 == y2 + 1 || y1 == y2 - 1)){
        action = 4*(4-1) + (y1-1 < y2-1 ? y1-1 : y2-1)*4 + (x1-1);
        return action;
    }
    else{
        //invalid action
        return -1;
    }
}

void XY_to_coordinates(uint8_t x, uint8_t y, uint8_t* x_, uint8_t* y_){
    if(x>4 || x<1 || y>4 || y<1){
        //invalid coordinates
        return;
    }
    *x_= (x-1) + x;
    *y_ = y;
}