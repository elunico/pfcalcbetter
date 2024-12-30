#!/bin/zsh

flags="-o main -I."

if [[ "$1" == "debug" || "$2" == "debug" ]]; then
    flags="$flags -g"
else
    flags="$flags -DNDEBUG"
fi

if [[ "$1" == "long" || "$2" == "long" ]]; then
    flags="$flags -DPF_NUM_LONG"
fi


if [[ "$1" == "double" || "$2" == "double" ]]; then
    flags="$flags -DPF_NUM_DOUBLE"
fi

eval cc $flags *.c

