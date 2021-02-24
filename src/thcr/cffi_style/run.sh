#!/usr/bin/env sh

set -e

g++ -o ./PokerHandInterface.so ./PokerHandOdds.cpp ./PokerHandBitFunction.cpp ./PokerHandEval.cpp ./PokerHandInterface.cpp -fPIC -shared