#!/bin/bash

set -e

[ -d networks ] && rm -r networks
mkdir -p networks

[ -f log ] && rm log

for i in {1..16}; do
    printf "net %s: " $i >>log
    /usr/bin/time -f %e ./ice4-ob 2>>log << CMDS
uci
setoption name Threads value 16
train
quit
CMDS
    cp network.txt networks/$i.txt
done

echo "{" >multinet.txt
for net in networks/*.txt; do
    cat "$net" >>multinet.txt
    echo "," >>multinet.txt
done
echo "}" >>multinet.txt
