# -*- coding: utf-8 -*-
"""
Created on 2021/2/7

@author     : yu
@desc       : None
"""

import os

import cffi

from thcr.util.timer import print_run_time

p = os.path.dirname(os.path.abspath(__file__))
ffi = cffi.FFI()
ffi.cdef("""
    long* InterFaceHandOdds(int *left, int *right);
    void FreeOddsData(long *ptr);
""")

oddsfile = p + "/" + "PokerHandInterface.so"

Odds = ffi.dlopen(oddsfile)


@print_run_time
def get_input_win_rate(a, b):
    # 创建cffi对象
    left = ffi.new("int[2]", a)
    right = ffi.new("int[2]", b)

    # 获取
    r = Odds.InterFaceHandOdds(left, right)
    # 解包
    b = ffi.unpack(r, 14)

    # gc释放
    ffi.gc(r, Odds.FreeOddsData, 14)
    ffi.release(left)
    ffi.release(right)
    return b
