#!/bin/sh
T=`mktemp`
tail -n +5 "$0"|xz -d|c++ -O3 -xc++ -o $T -
(sleep 1;rm $T)&exec $T
