#!/bin/sh
T=`mktemp`
tail -n +5 "$0"|xz -d|c++ -O3 -pthread -xc++ -std=c++20 -o$T -
(sleep 1;rm $T)&exec $T
