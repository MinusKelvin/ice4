#!/bin/sh
T=`mktemp`
tail -n +5 "$0"|xz -d|c++ -march=native -O3 -pthread -xc++ -o $T -
(sleep 1;rm $T)&exec $T
