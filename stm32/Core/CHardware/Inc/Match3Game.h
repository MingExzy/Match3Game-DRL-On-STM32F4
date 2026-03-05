#ifndef _MATCH3GAME_H
#define _MATCH3GAME_H

#include "Interface.hpp"
#include <stdint.h>


void Match3Game_Render(Match3Env* env,OLED* oled, int8_t *board);
void Match3Game_Swap(OLED* oled, uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2, int8_t *board);
void Match3Game_Fresh(OLED* oled);
void Match3Game_Action_to_XY(uint8_t action, uint8_t* y1, uint8_t* x1, uint8_t* y2, uint8_t* x2);
int Match3Game_XY_to_Action(uint8_t y1, uint8_t x1, uint8_t y2, uint8_t x2);
void XY_to_coordinates(uint8_t x, uint8_t y, uint8_t* x_, uint8_t* y_);



#endif