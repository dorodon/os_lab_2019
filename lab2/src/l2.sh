#!/bin/bash
#z1
cd /projects/dorodon-os_lab_2019/lab2/src/swap
gcc -c ./main.c -o ./main.o

#z3
cd /projects/dorodon-os_lab_2019/lab2/src/revert_string

mkdir dynamic
gcc -c -fPIC ./revert_string.c -o ./dynamic/revert_string.o
gcc -shared ./dynamic/revert_string.o -o ./dynamic/lib_revert_string.so
gcc /projects/dorodon-os_lab_2019/lab2/src/revert_string/main.o -L/projects/dorodon-os_lab_2019/lab2/src/revert_string/dynamic -l_revert_string -o /projects/dorodon-os_lab_2019/lab2/src/revert_string/dynamically_linked
LD_LIBRARY_PATH=/projects/dorodon-os_lab_2019/lab2/src/revert_string/dynamic
./dynamically_linked abcde

mkdir static
gcc -c ./revert_string.c -o ./static/revert_string.o
ar rcs ./static/lib_revert_string.a ./static/revert_string.o
gcc ./main.o -L/projects/dorodon-os_lab_2019/lab2/src/revert_string/static -l_revert_string -o ./statically_linked
./statically_linked sdfsaq

#z4
cd /projects/dorodon-os_lab_2019/lab2/src/tests

gcc -c ./tests.c -o ./tests.o
gcc ./tests.o -L/projects/dorodon-os_lab_2019/lab2/src/revert_string/dynamic -l_revert_string -o /projects/dorodon-os_lab_2019/lab2/src/tests/dynamically_linked
./dynamically_linked