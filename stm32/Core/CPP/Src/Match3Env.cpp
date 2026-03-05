#include "Match3Env.hpp"
#include <stdint.h>
#include <string.h>

Match3Env::Match3Env(){
    max_steps = 30;
    current_step = 0;
    
}

Match3Env::~Match3Env(){

}

void Match3Env::GetBoard(int8_t *board_){

    for(uint8_t i=0; i<board_size; i++){
        for(uint8_t j=0; j<board_size; j++){
            board_[i*board_size+j] = board[i][j];
        }
    }
}

void Match3Env::Reset(){
    int8_t board_[4][4]={{0}};
    for(uint8_t i=0; i<board_size; i++){
        for(uint8_t j=0; j<board_size; j++){
            uint8_t num;
            Myrandom_generate(&num, 1, elements_num);
            board[i][j] = num;
        }
    }
    while (check_match(board_[0])){
        resolve_match(board_[0]);
        memset(board_, 0, sizeof(board_));
    }
    current_step = 0;
}

void Match3Env::Step(uint8_t action,int32_t* reward, bool* done, char* info){
    current_step+=1;
    if(action<0 || action>= 2*board_size*(board_size-1)){
        //invalid action
        *done = true;
        *reward = -10;
        strcpy(info, "Invalid action");
        return;
    }

    EnvSwap(action);
    int8_t total_cleared = 0;
    int8_t board_[4][4]={{0}};

    while (true){
        if (!check_match(board_[0])){
            break;
        }
        for(uint8_t i=0; i<board_size; i++){
            for(uint8_t j=0; j<board_size; j++){
                if(board_[i][j] == 1){
                    total_cleared += 1;
                }
            }
        }
        resolve_match(board_[0]);
        memset(board_, 0, sizeof(board_));
    }

    *reward += total_cleared;
    if (total_cleared == 0){
        *reward += -1;
    }

    if(current_step >= max_steps){
        *done = true;
        strcpy(info, "GameOver");
    }

}


extern "C"{

    Match3Env* Match3Env_Create(void){
        return new Match3Env();
    }

    void Match3Env_Destroy(Match3Env* env){
        delete env;
    }

    void Match3Env_Reset(Match3Env* env){
        env->Reset();
    }

    void Match3Env_Step(Match3Env* env, uint8_t action,int32_t* reward, bool* done, char* info){
        env->Step(action, reward, done, info);
    }

    void Match3Env_GetBoard(Match3Env* env, int8_t *board_){
        env->GetBoard(board_);
    }

}