#!/bin/sh

if echo $1 | grep -q "racket" ; then
    raco exe scheme-compressor/rouleau.rkt
    mv scheme-compressor/rouleau bin/rouleau
elif echo $1 | grep -q "c++" ; then
    g++ -std=c++11 *.cpp -o bin/main
elif echo $1 | grep -q "clean" ; then
    rm -rf bin
    mkdir bin
else 
    raco exe scheme-compressor/rouleau.rkt
    mv scheme-compressor/rouleau bin/rouleau
    g++ -std=c++11 *.cpp -o bin/main
fi
