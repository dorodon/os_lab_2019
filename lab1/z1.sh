#!/bin/bash
#/projects/dorodon-os_lab_2019/lab1/z1.sh
clear
/projects/dorodon-os_lab_2019/lab1/src/background.sh &
echo "Lots of letters" > /projects/dorodon-os_lab_2019/lab1/file.txt
cat -E /projects/dorodon-os_lab_2019/lab1/file.txt
wc -m /projects/dorodon-os_lab_2019/lab1/file.txt