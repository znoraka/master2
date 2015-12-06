#!/bin/bash

if echo $1 | grep -q ".ppm" ||  echo $1 | grep -q ".pgm" ; then
    bin/main $1 $3 &&
    bin/rouleau -b out.tmp $2 &&
    ls -sh out.tmp &&
    ls -sh $2 &&
    taux=$(echo $(du -b $1 | awk '{print $1}') / $(du -b $2 | awk '{print $1}') | tr -dc '0-9/ ') &&
    echo "taux de compression =" $(echo "scale=2;" $taux | bc -l) &&
    rm out.tmp
else
    bin/rouleau -d $1 out.tmp && bin/main out.tmp $2 && eog $2 &&
    rm out.tmp
fi
