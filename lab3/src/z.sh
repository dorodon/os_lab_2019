#!/bin/bash
#z1
cd /projects/dorodon-os_lab_2019/lab3/src/
gcc -std=c99 find_min_max.c sequential_min_max.c utils.c
./a.out 2 6
#z2
cd /projects/dorodon-os_lab_2019/lab3/src/
gcc -std=c99 find_min_max.c parallel_min_max.c utils.c -o b.out
./b.out --seed 1 --array_size 6 --pnum 2