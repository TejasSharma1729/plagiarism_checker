#!/bin/bash
# This is for you, the student to execute

a=0
b=0
cd phase1
if ! make; then
    make clean
    a=0
else
    ./main > ../one.txt
    make clean
    a=`cat ../one.txt | grep "Total" | awk '{print $3}'`
fi
cd ../phase2
if ! make; then
    make clean
    b=0
else
    ./main > ../two.txt
    make clean
    b=`cat ../two.txt | grep "Total" | awk '{print $3}'`
fi
cd ../
result=$(python3 -c "print($a + $b)")
echo "RESULT: $result / 8"
