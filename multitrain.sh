#!/bin/bash

set -e

[ -d multinet ] && rm -r multinet
mkdir multinet

[ -f log ] && rm log

for i in {1..16}; do
    [ -d networks ] && rm -r networks
    mkdir networks
    printf "net %s: " $i >>log
    /usr/bin/time -f %e ./ice4-ob 2>>log << CMDS
uci
setoption name Threads value 16
train
quit
CMDS
    cp network.txt multinet/$i.txt
done

echo "{" >multinet.txt
for net in multinet/*.txt; do
    cat "$net" >>multinet.txt
    echo "," >>multinet.txt
done
echo "}" >>multinet.txt
