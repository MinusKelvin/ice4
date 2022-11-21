#!/bin/sh

if [ -z "$1" ]; then
    cargo run | xz -9 -e | cat launcher.sh - >ice4
    chmod +x ice4
    echo Compressed size: `du -b ice4`
else
    g++ -DOPENBENCH -O3 src/main.cpp -o "$1"
fi
