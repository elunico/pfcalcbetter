#!/bin/bash

flags="-o main -I."

if [[ "$1" == "debug" || "$2" == "debug" || "$3" == "debug" ]]; then
    flags="$flags -g -DNOPRINT"
else
    flags="$flags -DNDEBUG"
fi

if [[ "$1" == "long" || "$2" == "long" || "$3" == "long" ]]; then
    flags="$flags -DPF_NUM_LONG"
fi

if [[ "$1" == "double" || "$2" == "double" || "$3" == "double" ]]; then
    flags="$flags -DPF_NUM_DOUBLE"
fi

if [[ "$1" == "nozdiv" || "$2" == "nozdiv" || "$3" == "nozdiv"  ]]; then
    flags="$flags -DPFCALC_PREVENT_ZERO_DIV"
fi

echo cc $flags *.c

eval cc $flags *.c

