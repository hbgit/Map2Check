#!/bin/sh

python utils/cpplint.py --linelength=120 --counting=detailed $( find modules/ -name *.h -or -name *.cpp )
