#!/bin/bash
cd /projects/dorodon-os_lab_2019/lab3/src/
gcc -std=c99 find_min_max.c sequential_min_max.c utils.c
./a.out 1 4