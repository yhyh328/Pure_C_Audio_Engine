#!/usr/bin/env bash

tones=("220" "330" "440" "550" "660" "770" "880" "770" "660" "550" "440" "330" "220" "110") 
for tone in ${tones[@]}
do
    echo "$tone"
    ./pure_sine.exe 1 "$tone" 1.0
done

