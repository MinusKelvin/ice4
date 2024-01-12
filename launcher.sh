#!/bin/sh
T=`mktemp`
tail -n +5 "$0"|xz -d|c++ -O3 -march=native -pthread -xc++ -o$T -
(sleep 1;rm $T)&exec $T
