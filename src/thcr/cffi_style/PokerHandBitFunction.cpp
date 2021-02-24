//
//  PokerHandBitFunction.cpp
//  PokerHandOdds
//
//  Created by MalikMa on 16/7/4.
//  Copyright © 2016年 ninlgde. All rights reserved.
//

#include "PokerHandBitFunction.hpp"


int bitCount64(uint64_t x) {
//    int xf = bitCount32(uint32_t(x >> 32));
//    int xb = bitCount32(uint32_t(x));
    //    return xf + xb;
    x = (x & 0x5555555555555555) + ((x >> 1)  & 0x5555555555555555);
    x = (x & 0x3333333333333333) + ((x >> 2)  & 0x3333333333333333);
    x = (x & 0x0F0F0F0F0F0F0F0F) + ((x >> 4)  & 0x0F0F0F0F0F0F0F0F);
    x = (x & 0x00FF00FF00FF00FF) + ((x >> 8)  & 0x00FF00FF00FF00FF);
    x = (x & 0x0000FFFF0000FFFF) + ((x >> 16) & 0x0000FFFF0000FFFF);
    x = (x & 0x00000000FFFFFFFF) + ((x >> 32) & 0x00000000FFFFFFFF);
    return (int)x;
}

int bitCount32(uint32_t x) {
    x = (x & 0x55555555) + ((x >> 1)  & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2)  & 0x33333333);
    x = (x & 0x0F0F0F0F) + ((x >> 4)  & 0x0F0F0F0F);
    x = (x & 0x00FF00FF) + ((x >> 8)  & 0x00FF00FF);
    x = (x & 0x0000FFFF) + ((x >> 16) & 0x0000FFFF);
    return (int)x;
}

int bitCount16(uint16_t x) {
    x = (x & 0x5555) + ((x >> 1) & 0x5555);
    x = (x & 0x3333) + ((x >> 2) & 0x3333);
    x = (x & 0x0F0F) + ((x >> 4) & 0x0F0F);
    x = (x & 0x00FF) + ((x >> 8) & 0x00FF);
    return (int)x;
}