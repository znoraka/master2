#!/bin/sh

rm -rf bin
mkdir bin
raco exe scheme-compressor/rouleau.rkt
mv scheme-compressor/rouleau bin/rouleau
g++ -std=c++11 *.cpp -o bin/main
