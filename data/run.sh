#!/bin/bash
rm test.txt
sh test.sh $1 $2 $3
grep -i computation  ./test.txt > f1.txt 
#grep -i load ./test.txt > f2.txt
kate f1.txt
#kate f2.txt
