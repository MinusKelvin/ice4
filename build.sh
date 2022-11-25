#!/bin/sh

if [ -z "$1" ]; then
    cargo run | lzma -9 -e | cat launcher.sh - >ice4
    chmod +x ice4
    echo Compressed size: `du -b ice4`
else
    g++ -DOPENBENCH -march=x86-64-v3 -O3 src/main.cpp -o "$1"
fi
