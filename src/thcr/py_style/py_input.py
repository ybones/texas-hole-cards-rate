# -*- coding: utf-8 -*-

from itertools import combinations

from thcr.util.card import CardSet, CardHeap
from thcr.util.timer import print_run_time


@print_run_time
def get_input_win_rate(a, b):
    """
    获得胜率
    :param a:
    :param b:
    :return: [ 0.1234, 0.8943]
    """
    # 获取所有牌
    c = CardHeap()
    # 剔除a的牌
    c.popItems(a)
    # 剔除b的牌
    c.popItems(b)
    # 48张牌选5张 C(48, 5)
    r = combinations(c.cards, 5)
    aWin = 0
    bWin = 0
    tie = 0
    totalIndex = 0
    texasIndexList = [0 for _ in range(11)]
    for index, _r in enumerate(r):
        totalIndex += 1
        aCardList = a + list(_r)
        bCardList = b + list(_r)
        cardsList = [aCardList, bCardList]
        station = [0, 1]
        sets = [CardSet(cards, sit) for cards, sit in zip(cardsList, station)]
        sets = sorted(sets, reverse=True)
        max_sets = [s for s in sets if s == sets[0]]
        if len(max_sets) == 2:
            tie += 1
        elif max_sets[0].caller_data == 0:
            aWin += 1
        else:
            bWin += 1

        texasIndexList[max_sets[0].texas] += 1

    return [totalIndex] + texasIndexList[1:] + [aWin, bWin, tie]
