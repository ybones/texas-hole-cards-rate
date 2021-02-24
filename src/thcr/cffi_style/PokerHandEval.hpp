//
//  PokerHandEval.hpp
//  PokerHandOdds
//
//  Created by MalikMa on 16/7/4.
//  Copyright © 2016年 ninlgde. All rights reserved.
//

#ifndef PokerHandEval_hpp
#define PokerHandEval_hpp

#include <stdio.h>
#include <stdint.h>
#include "PokerHandData.h"
#include "PokerHandBitFunction.hpp"

uint64_t Evaluate(uint64_t cards, int numberOfCards);
int EvaluateHandType(uint64_t retval);

#endif /* PokerHandEval_hpp */
