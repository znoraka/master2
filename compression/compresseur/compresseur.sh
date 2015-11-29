#!/bin/sh

if echo $1 | grep -q ".noe" ; then
    ./rouleau -d out.noe out.tmp && ./main out.tmp && eog decoded.ppm
    rm out.tmp
else
    ../compiled/main $1 $2
    ../compiled/rouleau -b out.tmp out.noe
    ls -sh out.tmp
    ls -sh out.noe
    taux=$(echo $(du -b $1) / $(du -b out.noe) | tr -dc '0-9/ ')
    echo "taux de compression =" $(echo "scale=2;" $taux | bc -l)
    rm out.tmp
fi
