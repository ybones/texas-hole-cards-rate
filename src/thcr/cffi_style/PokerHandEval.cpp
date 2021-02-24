//
//  PokerHandEval.cpp
//  PokerHandOdds
//
//  Created by MalikMa on 16/7/4.
//  Copyright © 2016年 ninlgde. All rights reserved.
//

#include "PokerHandEval.hpp"

uint64_t Evaluate(uint64_t cards, int numberOfCards) {
    uint32_t retval = 0, four_mask, three_mask, two_mask;
    
    // Seperate out by suit
    uint16_t sc = (uint16_t)((cards >> (CLUB_OFFSET)) & 0x1fffUL);
    uint16_t sd = (uint16_t)((cards >> (DIAMOND_OFFSET)) & 0x1fffUL);
    uint16_t sh = (uint16_t)((cards >> (HEART_OFFSET)) & 0x1fffUL);
    uint16_t ss = (uint16_t)((cards >> (SPADE_OFFSET)) & 0x1fffUL);
    
    uint16_t ranks = sc | sd | sh | ss;
    uint16_t n_ranks = nBitsTable[ranks];
    uint16_t n_dups = ((uint16_t)(numberOfCards - n_ranks));
    
    /* Check for straight, flush, or straight flush, and return if we can
     determine immediately that this is the best possible hand
     */
    if (n_ranks >= 5)
    {
        if (nBitsTable[ss] >= 5)
        {
            if (straightTable[ss] != 0)
                return HANDTYPE_VALUE_STRAIGHTFLUSH + (uint32_t)(straightTable[ss] << TOP_CARD_SHIFT);
            else
                retval = HANDTYPE_VALUE_FLUSH + topFiveCardsTable[ss];
        }
        else if (nBitsTable[sc] >= 5)
        {
            if (straightTable[sc] != 0)
                return HANDTYPE_VALUE_STRAIGHTFLUSH + (uint32_t)(straightTable[sc] << TOP_CARD_SHIFT);
            else
                retval = HANDTYPE_VALUE_FLUSH + topFiveCardsTable[sc];
        }
        else if (nBitsTable[sd] >= 5)
        {
            if (straightTable[sd] != 0)
                return HANDTYPE_VALUE_STRAIGHTFLUSH + (uint32_t)(straightTable[sd] << TOP_CARD_SHIFT);
            else
                retval = HANDTYPE_VALUE_FLUSH + topFiveCardsTable[sd];
        }
        else if (nBitsTable[sh] >= 5)
        {
            if (straightTable[sh] != 0)
                return HANDTYPE_VALUE_STRAIGHTFLUSH + (uint32_t)(straightTable[sh] << TOP_CARD_SHIFT);
            else
                retval = HANDTYPE_VALUE_FLUSH + topFiveCardsTable[sh];
        }
        else
        {
            uint16_t st = straightTable[ranks];
            if (st != 0)
                retval = HANDTYPE_VALUE_STRAIGHT + (st << TOP_CARD_SHIFT);
        };
        
        /*
         Another win -- if there can't be a FH/Quads (n_dups < 3),
         which is true most of the time when there is a made hand, then if we've
         found a five card hand, just return.  This skips the whole process of
         computing two_mask/three_mask/etc.
         */
        if (retval != 0 && n_dups < 3)
            return retval;
    }
    
    /*
     * By the time we're here, either:
     1) there's no five-card hand possible (flush or straight), or
     2) there's a flush or straight, but we know that there are enough
     duplicates to make a full house / quads possible.
     */
    switch (n_dups)
    {
        case 0:
            /* It's a no-pair hand */
            return HANDTYPE_VALUE_HIGHCARD + topFiveCardsTable[ranks];
        case 1:
        {
            /* It's a one-pair hand */
            uint16_t t, kickers;
            
            two_mask = ranks ^ (sc ^ sd ^ sh ^ ss);
            retval = HANDTYPE_VALUE_PAIR;
            retval += (uint32_t) (topCardTable[two_mask] << TOP_CARD_SHIFT);
            t = ranks ^ two_mask;      /* Only one bit set in two_mask */
            /* Get the top five cards in what is left, drop all but the top three
             * cards, and shift them by one to get the three desired kickers */
            kickers = (topFiveCardsTable[t] >> CARD_WIDTH) & ~FIFTH_CARD_MASK;
            retval += kickers;
            return retval;
        }
            
        case 2:
            /* Either two pair or trips */
            two_mask = ranks ^ (sc ^ sd ^ sh ^ ss);
            if (two_mask != 0)
            {
                uint16_t t = ranks ^ two_mask; /* Exactly two bits set in two_mask */
                retval = HANDTYPE_VALUE_TWOPAIR;
                retval += (uint32_t) ((topFiveCardsTable[two_mask]
                                        & (TOP_CARD_MASK | SECOND_CARD_MASK))
                                     + (topCardTable[t] << THIRD_CARD_SHIFT));
                
                return retval;
            }
            else
            {
                uint16_t t, second;
                three_mask = ((sc & sd) | (sh & ss)) & ((sc & sh) | (sd & ss));
                retval = HANDTYPE_VALUE_TRIPS;
                retval += (uint32_t) (topCardTable[three_mask] << TOP_CARD_SHIFT);
                t = ranks ^ three_mask; /* Only one bit set in three_mask */
                second = topCardTable[t];
                retval += (second << SECOND_CARD_SHIFT);
                t ^= (1U << (int)second);
                retval += (uint16_t) (topCardTable[t] << THIRD_CARD_SHIFT);
                return retval;
            }
            
        default:
            /* Possible quads, fullhouse, straight or flush, or two pair */
            four_mask = sh & sd & sc & ss;
            if (four_mask != 0)
            {
                uint16_t tc = topCardTable[four_mask];
                retval = HANDTYPE_VALUE_FOUR_OF_A_KIND;
                retval += (uint32_t) ((tc << TOP_CARD_SHIFT)
                                     + ((topCardTable[ranks ^ (1U << (int)tc)]) << SECOND_CARD_SHIFT));
                return retval;
            };
            
            /* Technically, three_mask as defined below is really the set of
             bits which are set in three or four of the suits, but since
             we've already eliminated quads, this is OK */
            /* Similarly, two_mask is really two_or_four_mask, but since we've
             already eliminated quads, we can use this shortcut */
            
            two_mask = ranks ^ (sc ^ sd ^ sh ^ ss);
            if (nBitsTable[two_mask] != n_dups)
            {
                /* Must be some trips then, which really means there is a
                 full house since n_dups >= 3 */
                uint16_t tc, t;
                three_mask = ((sc & sd) | (sh & ss)) & ((sc & sh) | (sd & ss));
                retval = HANDTYPE_VALUE_FULLHOUSE;
                tc = topCardTable[three_mask];
                retval += (tc << TOP_CARD_SHIFT);
                t = (two_mask | three_mask) ^ (1U << (int)tc);
                retval += (uint32_t) (topCardTable[t] << SECOND_CARD_SHIFT);
                return retval;
            };
            
            if (retval != 0) /* flush and straight */
                return retval;
            else
            {
                /* Must be two pair */
                uint16_t top, second;
                
                retval = HANDTYPE_VALUE_TWOPAIR;
                top = topCardTable[two_mask];
                retval += (top << TOP_CARD_SHIFT);
                second = topCardTable[two_mask ^ (1 << (int)top)];
                retval += (second << SECOND_CARD_SHIFT);
                retval += (uint32_t) ((topCardTable[ranks ^ (1U << (int)top) ^ (1 << (int)second)]) << THIRD_CARD_SHIFT);
                return retval;
            }
    }
}

int EvaluateHandType(uint64_t retval) {
    int type = (retval & HANDTYPE_MASK) >> HANDTYPE_SHIFT;
    if (type == StraightFlush && (retval & TOP_CARD_MASK) >> TOP_CARD_SHIFT == RankA)
        type++;
    return type;
}
