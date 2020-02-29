#!/bin/bash
#/projects/dorodon-os_lab_2019/lab1/z4.sh
rm /projects/dorodon-os_lab_2019/lab1/numbers.txt 2> /dev/null
for i in {1..3}
do
    head -c1 /dev/random | od -i | awk '{print $2}' >> /projects/dorodon-os_lab_2019/lab1/numbers.txt
done