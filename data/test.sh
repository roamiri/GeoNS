#!/bin/bash
END=$1  # number of iterations
for i in $(seq 1 $END);
 do 
echo $i;
./pathSelection --area $2 --tree $3 >> test.txt;
done

