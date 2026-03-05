# ifndef Match3Env_hpp
# define Match3Env_hpp
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include "RandomGenerate.h"

class Match3Env{
public:
    int8_t board[4][4];
    Match3Env();
    ~Match3Env();
    void Render();
    void Step(uint8_t action,int32_t* reward, bool* done, char* info);
    void Reset();
    void GetBoard(int8_t *board_);
private:
    uint8_t max_steps;
    uint8_t current_step;
    uint8_t elements_num = 3;
    uint8_t board_size = 4;

    void EnvSwap(uint8_t action){
        if(action>=0 && action< board_size*(board_size-1)){
            uint8_t x1 = action / (board_size-1);
            uint8_t y1 = action % (board_size-1);
            uint8_t x2 = x1;
            uint8_t y2 = y1 + 1;
            uint8_t temp = board[x1][y1];
            board[x1][y1] = board[x2][y2];
            board[x2][y2] = temp;
        }
        else if(action>=board_size*(board_size-1) && action< 2*board_size*(board_size-1)){
            uint8_t action_ = action - board_size*(board_size-1);
            uint8_t x1 = action_ / board_size;
            uint8_t y1 = action_ % board_size;
            uint8_t x2 = x1 + 1;
            uint8_t y2 = y1;
            uint8_t temp = board[x1][y1];
            board[x1][y1] = board[x2][y2];
            board[x2][y2] = temp;
        }
        else{
            //invalid action
        }
    }

    bool check_match(int8_t *board_){
        bool match = false;

        for(uint8_t i=0; i<board_size; i++){
            for(uint8_t j=0; j<board_size-2; j++){
                if(board[i][j] == board[i][j+1] && board[i][j+1] == board[i][j+2]){
                    match = true;
                    board_[i*board_size+j] = 1;
                    board_[i*board_size+j+1] = 1;
                    board_[i*board_size+j+2] = 1;
                }
            }
        }

        for(uint8_t j=0; j<board_size; j++){
            for(uint8_t i=0; i<board_size-2; i++){
                if(board[i][j] == board[i+1][j] && board[i+1][j] == board[i+2][j]){
                    match = true;
                    board_[i*board_size+j] = 1;
                    board_[i*board_size+j+1] = 1;
                    board_[i*board_size+j+2] = 1;
                }
            }
        }
        return match;
    }

    void resolve_match(int8_t *board_){
        if(board_ == NULL){
            return;
        }
        for(uint8_t i=0; i<board_size; i++){
            for(uint8_t j=0; j<board_size; j++){
                if(board_[i*board_size+j] == 1){
                    board[i][j] = -1;
                }
            }
        }

        for(uint8_t j=0; j<board_size; j++){
            uint8_t empty_count = 0;
            for(int8_t i=board_size - 1; i>=0;i--){
                if(board[i][j] == -1){
                    empty_count += 1;
                }
                else if(empty_count > 0){
                    board[i+empty_count][j] = board[i][j];
                    board[i][j] = -1;
                }
            }
        }

        for (uint8_t j=0; j<board_size; j++){
            for(uint8_t i=0; i<board_size; i++){
                if(board[i][j] == -1){
                    uint8_t num;
                    Myrandom_generate(&num, 1, elements_num);
                    board[i][j] = num;
                }
            }
        }

    }
};




#endif