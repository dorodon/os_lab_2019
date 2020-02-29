#!/bin/bash
#/projects/dorodon-os_lab_2019/lab1/average.sh
N=$#;

echo "N: $N"
count=1
S=0
while [[ $# -gt 0 ]]
do
    #echo "arg $count = $1"
    count=$((count+1))
    S=$(($S+$1))
    shift
done
avg=$(($S/$N))
echo "avg: $avg"