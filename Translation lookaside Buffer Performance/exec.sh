#!/bin/bash

echo -e "Execute tlb with 2 till 100 pages and  100 till 1000 trials"
[ -e tlb ] && echo "tlb compiled." || gcc -O0 tlb.c -o tlb -pthread
[ -e output.txt ] && rm -rf output.txt
for i in $(seq 2 2 100)
	do
		for j in $(seq 100 100 1000);
		do
			./tlb $i $j >> output.txt
		done
	done
