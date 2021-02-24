//
//  PokerHandOdds.hpp
//  PokerHandOdds
//
//  Created by MalikMa on 16/7/4.
//  Copyright © 2016年 ninlgde. All rights reserved.
//

#ifndef PokerHandOdds_hpp
#define PokerHandOdds_hpp

#include <stdio.h>
#include <stdint.h>
#include <string>
#include "PokerHandData.h"
#include "PokerHandEval.hpp"
#include "PokerHandBitFunction.hpp"

class PokerHandOdds {
    
public:
    
    void Hands(uint64_t *pocketmasks, uint64_t shared, uint64_t dead, int numberOfCards, const int playerNum, long wins[], long losses[], long ties[], long &totalHands, long types[]);
    
    void EvalHands(uint64_t *pocketmasks, uint64_t boardhand, const int playerNum, long wins[], long losses[], long ties[], long &totalHands, long types[]);
    
    bool ValidateHand(const std::string cards);
    
    uint64_t ParseHand(const std::string hand, int &count);
    uint64_t ParseHand(const std::string hand, const std::string board, int &count);
    
    int NextCard(const std::string cards, int &index);
    
    void HandOdds(const std::string *player, const int playerNum, const std::string board, const std::string dead, long wins[], long losses[], long ties[], long &total, long types[]);
    
    void HandOdds(const int (*player)[2], const int playerNum, const int board[], const int boardLen, const int dead[], const int deadLen, long wins[], long losses[], long ties[], long &total, long types[]);
    
    uint64_t ParseHand(const int *hand, const int handLen);
};

#endif /* PokerHandOdds_hpp */
