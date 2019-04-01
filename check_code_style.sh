#!/bin/sh

python utils/cpplint.py --headers=hpp,h --linelength=120 --counting=detailed $( find modules/frontend -name *.h -or -name *.hpp -or -name *.cpp )
