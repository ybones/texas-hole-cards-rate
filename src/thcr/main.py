# -*- coding: utf-8 -*-
from thcr.cffi_style import cffi_input
from thcr.py_style import py_input
from thcr.util.card import CardHeap

if __name__ == "__main__":
    c = CardHeap()
    left = c.getCardList(2)
    right = c.getCardList(2)
    numListCffi = cffi_input.get_input_win_rate(left, right)
    print (numListCffi)
    numList = py_input.get_input_win_rate(left, right)
    print (numList)
