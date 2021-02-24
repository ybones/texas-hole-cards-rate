//
//  PokerHandInterface.cpp
//  PokerHandOdds
//
//  Created by 于庆宇 on 2021/2/5.
//  Copyright © 2021 ninlgde. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "PokerHandOdds.hpp"

long* _InterFaceHandOdds(int *left, int *right)
{
    const int COUNT = 2;

    PokerHandOdds pho;

    int pockets[COUNT][2] = {{left[0], left[1]}, {right[0], right[1]}};

    int board[] = {};
    int dead[] = {};
    long wins[COUNT] = {0};
    long losses[COUNT] = {0};
    long ties[COUNT] = {0};
    long types[10] = {0};
    long total = 0;

    pho.HandOdds(pockets, COUNT, board, 0, dead, 0, wins, losses, ties, total, types);

    // total 0
    // types 1 ~ 10:
    // win 11, 12
    // tie 13
    long *odds = (long*)malloc(sizeof(long)*14);
    odds[0] = total;
    memcpy(&odds[1], types, sizeof(types));
    memcpy(&odds[11], wins, sizeof(wins));
    odds[13] = ties[0];

    return odds;
}

extern "C"
{
    extern long* InterFaceHandOdds(int *left, int *right)
    {
        return _InterFaceHandOdds(left, right);
    }

    extern void FreeOddsData(long *ptr)
    {
        free(ptr);
    }
}
