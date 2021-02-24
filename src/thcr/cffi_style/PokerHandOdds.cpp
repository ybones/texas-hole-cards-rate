//
//  PokerHandOdds.cpp
//  PokerHandOdds
//
//  Created by MalikMa on 16/7/4.
//  Copyright © 2016年 ninlgde. All rights reserved.
//

#include "PokerHandOdds.hpp"

void PokerHandOdds::HandOdds(const int (*player)[2], const int playerNum, const int board[], const int boardLen, const int dead[], const int deadLen, long wins[], long losses[], long ties[], long &total, long types[]) {
    uint64_t pocketmasks[9];
    int count = 0;
    uint64_t boardmask = 0, deadcards_mask = 0, deadcards = ParseHand(dead, deadLen);
    
    total = 0;
    deadcards_mask |= deadcards;
    
    // Read pocket cards
    for (int i = 0; i < playerNum; i++)
    {
        count = 0;
        pocketmasks[i] = ParseHand(player[i], 2);
        deadcards_mask |= pocketmasks[i];
        wins[i] = ties[i] = losses[i] = 0;
    }
    
    // Read board cards
    count = 0;
    boardmask = ParseHand(board, boardLen);
    
    Hands(pocketmasks, boardmask, deadcards_mask, 5, playerNum, wins, losses, ties, total, types);
    
}

uint64_t PokerHandOdds::ParseHand(const int *hand, const int handLen) {
    uint64_t handmask = 0UL;
    if (handLen == 0) {
        return 0ULL;
    }
    
    for (int i = 0; i < handLen; i++) {
        int rank = (hand[i] % 100) - 2;
        int suit = hand[i] / 100 - 1;
        int card = rank + (suit * 13);
        handmask |= (1UL << card);
    }
    return handmask;
}

void PokerHandOdds::HandOdds(const std::string *player, const int playerNum, const std::string board, const std::string dead, long wins[], long losses[], long ties[], long &totalHands, long types[]) {
    uint64_t pocketmasks[9];
    int count = 0;
    uint64_t boardmask = 0, deadcards_mask = 0, deadcards = ParseHand(dead, count);
    
    totalHands = 0;
    deadcards_mask |= deadcards;
    
    // Read pocket cards
    for (int i = 0; i < playerNum; i++)
    {
        count = 0;
        pocketmasks[i] = ParseHand(player[i], "", count);
        deadcards_mask |= pocketmasks[i];
        wins[i] = ties[i] = losses[i] = 0;
    }
    
    // Read board cards
    count = 0;
    boardmask = ParseHand("", board, count);
    
    Hands(pocketmasks, boardmask, deadcards_mask, 5, playerNum, wins, losses, ties, totalHands, types);
    
}

int PokerHandOdds::NextCard(const std::string cards, int &index) {
    int rank = 0, suit = 0;
    while (index < cards.length() && cards[index] == ' ')
        index++;
    
    if (index >= cards.length())
        return -1;
    
    // Parse cards
    if (index < cards.length())
    {
        switch (cards[index++])
        {
            case '1':
                if (cards[index] == '0')
                {
                    index++;
                    rank = RankT;
                }
                else
                {
                    return -1;
                }
                break;
            case '2':
                rank = Rank2;
                break;
            case '3':
                rank = Rank3;
                break;
            case '4':
                rank = Rank4;
                break;
            case '5':
                rank = Rank5;
                break;
            case '6':
                rank = Rank6;
                break;
            case '7':
                rank = Rank7;
                break;
            case '8':
                rank = Rank8;
                break;
            case '9':
                rank = Rank9;
                break;
            case 'T':
            case 't':
                rank = RankT;
                break;
            case 'J':
            case 'j':
                rank = RankJ;
                break;
            case 'Q':
            case 'q':
                rank = RankQ;
                break;
            case 'K':
            case 'k':
                rank = RankK;
                break;
            case 'A':
            case 'a':
                rank = RankA;
                break;
            default:
                return -2;
        }
    }
    else
    {
        return -2;
    }
    
    if (index < cards.length())
    {
        switch (cards[index++])
        {
            case 'H':
            case 'h':
                suit = Hearts;
                break;
            case 'D':
            case 'd':
                suit = Diamonds;
                break;
            case 'C':
            case 'c':
                suit = Clubs;
                break;
            case 'S':
            case 's':
                suit = Spades;
                break;
            default:
                return -2;
        }
    }
    else
    {
        return -2;
    }
    
    return rank + (suit * 13);
    return 0;
}

uint64_t PokerHandOdds::ParseHand(const std::string hand, const std::string board, int &count) {
    std::string cards = hand + board;
    return ParseHand(cards, count);
}

uint64_t PokerHandOdds::ParseHand(const std::string hand, int &count) {
    int index = 0;
    int cards;
    uint64_t handmask = 0UL;
    if (hand.length() == 0) {
        count = 0;
        return 0ULL;
    }
//    if (!ValidateHand(cards)) return 0UL;
    cards = 0;
    for (int card = NextCard(hand, index); card >= 0; card = NextCard(hand, index))
    {
        handmask |= (1UL << card);
        cards++;
    }
    return handmask;
}

bool PokerHandOdds::ValidateHand(const std::string hand) {
    int index = 0;
    uint64_t handmask = 0UL;
    int cards = 0;
    int card = 0;
    
    for (card = NextCard(hand,index); card >= 0; card = NextCard(hand,index))
    {
        if ((handmask & (1UL << card)) != 0)
            return false;
        handmask |= (1UL << card);
        cards++;
    }
    
    return card == -1 && cards > 0 && index >= hand.length();
}

inline void PokerHandOdds::EvalHands(uint64_t *pocketmasks, uint64_t boardhand, const int playerNum, long *wins, long *losses, long *ties, long &totalHands, long types[]) {
    uint64_t pockethands[9];
    int bestcount;
    // Evaluate all hands and determine the best hand
    uint64_t bestpocket = Evaluate(pocketmasks[0] | boardhand, 7);
    pockethands[0] = bestpocket;
    bestcount = 1;
    for (int i = 1; i < playerNum; i++)
    {
        pockethands[i] = Evaluate(pocketmasks[i] | boardhand, 7);
        if (pockethands[i] > bestpocket)
        {
            bestpocket = pockethands[i];
            bestcount = 1;
        }
        else if (pockethands[i] == bestpocket)
        {
            bestcount++;
        }
    }
    
    int besttype = EvaluateHandType(bestpocket);
    types[besttype]++;
    
    // Calculate wins/ties/loses for each pocket + board combination.
    for (int i = 0; i < playerNum; i++)
    {
        if (pockethands[i] == bestpocket)
        {
            if (bestcount > 1)
            {
                ties[i]++;
            }
            else
            {
                wins[i]++;
            }
        }
        else if (pockethands[i] < bestpocket)
        {
            losses[i]++;
        }
    }
    
    totalHands++;
}

void PokerHandOdds::Hands(uint64_t *pocketmasks, uint64_t shared, uint64_t dead, int numberOfCards, const int playerNum, long wins[], long losses[], long ties[], long &totalHands, long types[]) {
    int _i1, _i2, _i3, _i4, _i5, length;
    uint64_t _card1, _card2, _card3, _card4, _card5;
    uint64_t _n2, _n3, _n4;
    
    uint64_t boardhand;
    
    dead |= shared;
    
    switch (numberOfCards - bitCount64(shared)) {
        case 5:
            for (_i1 = NumberOfCards - 1; _i1 >= 0; _i1--)
            {
                _card1 = CardMasksTable[_i1];
                if ((dead & _card1) != 0) continue;
                for (_i2 = _i1 - 1; _i2 >= 0; _i2--)
                {
                    _card2 = CardMasksTable[_i2];
                    if ((dead & _card2) != 0) continue;
                    _n2 = _card1 | _card2;
                    for (_i3 = _i2 - 1; _i3 >= 0; _i3--)
                    {
                        _card3 = CardMasksTable[_i3];
                        if ((dead & _card3) != 0) continue;
                        _n3 = _n2 | _card3;
                        for (_i4 = _i3 - 1; _i4 >= 0; _i4--)
                        {
                            _card4 = CardMasksTable[_i4];
                            if ((dead & _card4) != 0) continue;
                            _n4 = _n3 | _card4;
                            for (_i5 = _i4 - 1; _i5 >= 0; _i5--)
                            {
                                _card5 = CardMasksTable[_i5];
                                if ((dead & _card5) != 0) continue;
                                boardhand = _n4 | _card5 | shared;
                                EvalHands(pocketmasks, boardhand, playerNum, wins, losses, ties, totalHands, types);
                            }
                        }
                    }
                }
            }
            break;
        case 2:
            GET_TABLE_LENGTH(TwoCardTable, length);
            for (_i1 = 0; _i1 < length; _i1++)
            {
                _card1 = TwoCardTable[_i1];
                if ((dead & _card1) != 0) continue;
                boardhand = _card1 | shared;
                EvalHands(pocketmasks, boardhand, playerNum, wins, losses, ties, totalHands, types);
            }
            break;
        case 1:
            GET_TABLE_LENGTH(CardMasksTable, length);
            for (_i1 = 0; _i1 < length; _i1++)
            {
                _card1 = CardMasksTable[_i1];
                if ((dead & _card1) != 0) continue;
                boardhand = _card1 | shared;
                EvalHands(pocketmasks, boardhand, playerNum, wins, losses, ties, totalHands, types);
            }
            break;
    }
    
}
