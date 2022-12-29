#!/bin/bash

input=`mktemp`
cat > $input

compress() {
    xz --format=lzma --lzma1=$1
}

best_params=""
best_size=99999
for mf in hc3 hc4 bt2 bt3 bt4; do
    for nice in {4..273}; do
        params="preset=9e,lc=2,lp=0,pb=0,mf=$mf,nice=$nice"
        size=$(compress $params < $input | wc -c)
        if (( size < best_size )); then
            best_params=$params
            best_size=$size
        fi
    done
done

echo "Compressing using params $best_params" >&2
compress $best_params < $input

rm $input
