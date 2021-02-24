# -*- coding: utf-8 -*-

from itertools import groupby
from operator import itemgetter


class Texas(object):
    # types 1 ~ 10:
    HIGH_CARD, ONE_PAIR, TWO_PAIRS, THREE_OF_A_KIND, STRAIGHT, FLUSH, \
    FULL_HOUSE, FOUR_OF_A_KIND, STRAIGHT_FLUSH, ROYAL_FLUSH = range(1, 11)

    upper_names = ("_", "HIGH CARD", "ONE PAIR", "TWO PAIRS", "THREE OF A KIND",
                   "STRAIGHT", "FLUSH", "FULL HOUSE", "FOUR OF A KIND",
                   "STRAIGHT FLUSH", "ROYAL FLUSH")


class Card(object):
    '''一张牌'''

    def __init__(self, _type, number):
        self.t, self.n = _type, number

    @staticmethod
    def cmp(a, b):
        """ 两张牌比较，只比较牌值，不管花色。"""
        return a.n - b.n

    def __str__(self):
        return "%d%02d" % (self.t, self.n)

    def __repr__(self):
        return self.__str__()

    def __int__(self):
        return self.t * 100 + self.n

    def toShortInt(self):
        ''' 102 ~ 414 => 1 ~ 52 '''
        return (self.t - 1) * 13 + (1 if self.n == 14 else self.n)

    @staticmethod
    def _split_card_value(int_card):
        """ _split_card_value(103) --> (1, 3) """

        if int_card > 400:
            return (4, int_card - 400)
        elif int_card > 300:
            return (3, int_card - 300)
        elif int_card > 200:
            return (2, int_card - 200)
        else:
            return (1, int_card - 100)

    @classmethod
    def _splitShort(cls, int_card):
        ''' 1 ~ 52 => t, n '''
        if int_card > 39:
            t, n = 4, int_card - 39
        elif int_card > 26:
            t, n = 3, int_card - 26
        elif int_card > 13:
            t, n = 2, int_card - 13
        else:
            t, n = 1, int_card

        if n == 1:
            n = 14
        return t, n

    @classmethod
    def fromInt(cls, int_card):
        return cls(*Card._split_card_value(int_card))

    @classmethod
    def intToShort(cls, int_card):
        ''' 102 ~ 414 => 1 ~ 52 '''
        t, n = cls._split_card_value(int_card)
        return (t - 1) * 13 + (1 if n == 14 else n)

    @classmethod
    def shortToInt(cls, int_card):
        ''' 1 ~ 52 => 102 ~ 414 '''
        t, n = cls._splitShort(int_card)
        return t * 100 + (14 if n == 1 else n)


class CardSet(object):
    '''一手牌'''

    def __init__(self, int_cards, caller_data=None):
        if len(int_cards) < 5:
            raise Exception("len Cards < 5. cards caller_data: %s, %s" % (int_cards, caller_data))

        self.caller_data = caller_data
        self.int_cards = int_cards  # 111, 211, 311, 113, 313, 312, 112
        self.cards = None  # list: [Card, Card, Card, ... ]
        self.group_by_t = {}  # {1:(J,K,Q), 2:(J,), 3:(J,Q,K), 4:()}
        self.group_by_n = {}  # {K:(1,3), J:(2,1,3), Q:(3,1)}

        # these two lists are generate from "group_by_n":
        self.order_by_number = []  # [(1K,3K)), (1Q,3Q)), (1J,2J,3J))]
        self.order_by_count = []  # [(1J,2J,3J), (1K,3K), (1Q,3Q)]

        self.best = []  # J J J K K;     Our result

        self._sort_cards()
        self.texas, self._best = self._calc_best()
        self.best = map(int, self._best)
        self.score = self._calc_score()

    def _sort_cards(self):
        """ 整理一下原始的牌，构造数据结构，为判断牌型做准备。"""
        self.cards = sorted(map(Card.fromInt, self.int_cards), reverse=True, cmp=Card.cmp)
        for c in self.cards:
            self.group_by_t[c.t] = self.group_by_t.get(c.t, ()) + (c,)
            self.group_by_n[c.n] = self.group_by_n.get(c.n, ()) + (c,)

        gbn = self.group_by_n
        self.order_by_number = [gbn[n] for n in sorted(gbn.keys(), reverse=True)]
        self.order_by_count = sorted(self.order_by_number, key=lambda (ts): -len(ts))

    def __str__(self):
        return "(priv:%s, type:%s, cards:%s)" % (self.caller_data, self.texas, self._best)

    def __repr__(self):
        return self.__str__()

    def __cmp__(self, other):
        """ 牌型比较 """
        if self.texas != other.texas:
            return self.texas - other.texas
        # 获取所有牌的 numbers 列表进行比较
        getn = lambda (_best): map(lambda c: c.n, _best)
        return cmp(getn(self._best), getn(other._best))  # texas type is same

    def _seqs(self, numbers):
        """ 获取连续牌 e.g. 1234689 --> [[1234],[6],[89]]"""
        renumerate = lambda (iterable): reversed(list(enumerate(reversed(iterable))))
        return [map(itemgetter(1), g) for k, g
                in groupby(renumerate(numbers), lambda (i, c): i - c.n)]

    def _get_max_seq(self, numbers):
        """ check the numbers has seq and great than 5 """
        return max(self._seqs(numbers), key=lambda (seq): len(seq))

    def _the_best(self, best):
        """ return the best 5 cards of all cards. """
        if len(best) >= 5:
            return list(best)[:5]
        remain = [x for x in self.cards if x not in best]
        return (list(best) + sorted(remain, reverse=True, cmp=Card.cmp))[:5]

    def _calc_best(self):
        # 查找同一花色多于或者等于 5 张牌的，如果能找到，那只可能有一种花色
        gt5 = filter(lambda (ns): len(ns) >= 5, self.group_by_t.values())
        if gt5:
            gt5 = sorted(gt5[0], reverse=True, cmp=Card.cmp)
            seq = self._get_max_seq(gt5)  # 取得最长的顺子

            if len(seq) >= 5 and seq[0].n == 14:  # 皇家同花顺
                return (Texas.ROYAL_FLUSH, self._the_best(seq[:5]))
            elif len(seq) >= 5:  # 同花顺
                return (Texas.STRAIGHT_FLUSH, self._the_best(seq[:5]))
            elif len(seq) == 4 and seq[-1].n == 2 and gt5[0].n == 14:  # 5432A
                return (Texas.STRAIGHT_FLUSH, self._the_best(seq[:4] + [gt5[0]]))

        if len(self.order_by_count[0]) == 4:  # 四条
            return (Texas.FOUR_OF_A_KIND, self._the_best(self.order_by_count[0]))
        elif len(self.order_by_count[0]) == 3 and len(self.order_by_count[1]) >= 2:  # 葫芦
            return (Texas.FULL_HOUSE,
                    self._the_best(self.order_by_count[0] + self.order_by_count[1]))

        if gt5:  # 同花
            return Texas.FLUSH, self._the_best(gt5[:5])

        if len(self.group_by_n) >= 5:
            uniqued = [x[0] for x in self.group_by_n.values()]  # 去重复
            if len(uniqued) >= 5:
                uniqued = sorted(uniqued, cmp=Card.cmp, reverse=True)
                seq = self._get_max_seq(uniqued)
                if len(seq) >= 5:  # 顺子
                    return Texas.STRAIGHT, self._the_best(seq[:5])
                elif len(seq) == 4 and seq[-1].n == 2 and uniqued[0].n == 14:  # 5432A
                    return (Texas.STRAIGHT, self._the_best(seq[:4] + [uniqued[0]]))

        if len(self.order_by_count[0]) == 3:  # 三条
            return Texas.THREE_OF_A_KIND, self._the_best(self.order_by_count[0])
        if len(self.order_by_count[0]) == 2 and len(self.order_by_count[1]) == 2:  # 两对
            return (Texas.TWO_PAIRS,
                    self._the_best(self.order_by_count[0] + self.order_by_count[1]))
        if len(self.order_by_count[0]) == 2:  # 对子
            return Texas.ONE_PAIR, self._the_best(self.order_by_count[0])

        # 高牌
        return Texas.HIGH_CARD, self._the_best(self.order_by_number[0])

    def _calc_score(self):
        try:
            return int('%02s%s' % (self.texas, ''.join(['%02d' % c.n for c in self._best])))
        except:
            return 0


class CardHeap(object):
    def __init__(self):
        self.cards = None
        self.initialize()

    def initialize(self):
        """
        初始化一副牌
        """
        self.cards = [int(Card(t, n)) for t in range(1, 5) for n in range(2, 15)]

    def getCardList(self, num):
        if len(self.cards) < num:
            return []

        return [int(self.cards.pop(0)) for _ in range(num)]

    def popItem(self, card):
        try:
            self.cards.remove(int(card))
        except:
            pass

    def popItems(self, cardList):
        for card in cardList:
            self.popItem(card)
