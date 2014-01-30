#!/bin/bash

HERE=$(dirname $0)

if [ -r $HERE/rsa_max.cpp ]; then
    g++ -lgmp -Wall -std=c++11 -lboost_regex -o $HERE/rsa_max $HERE/rsa_max.cpp
fi