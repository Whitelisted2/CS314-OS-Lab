#!/bin/bash

if [ "$1" == "1" ]; then
    echo "... part1 ..."
    make part1
    pnmtopng output_part1.ppm > result.png
elif [ "$1" == "2_1a" ]; then
    echo "... part2_1a ..."
    make part2_1a
    pnmtopng output_part2_1a.ppm > result.png
elif [ "$1" == "2_1b" ]; then
    echo "... part2_1b ..."
    make part2_1b
    pnmtopng output_part2_1b.ppm > result.png
elif [ "$1" == "2_2" ]; then
    echo "... part2_2 ..."
    make part2_2
    pnmtopng output_part2_2.ppm > result.png
elif [ "$1" == "2_3" ]; then
    echo "... part2_3 ..."
    make part2_3
    pnmtopng output_part2_3.ppm > result.png
else
    echo "part does not exist ... "
fi
make clean