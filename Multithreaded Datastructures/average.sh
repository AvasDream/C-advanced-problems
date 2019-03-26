#!/bin/bash
# Execute programm given as $arg1 $arg2 times and return average execution time
if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
    echo "Usage:"
    echo "./average <ProgrammName> <Retrys>"
    exit 1
fi
for i in $(seq 0 $2)
do
  ./$1 >> tmp.lst
done
#Curate raw time data
cat tmp.lst | grep Time | cut -d" " -f2 > time.raw
# Calculate Average
average=$(awk '{ total += $1; count++ } END { print total/count }' time.raw)
rm -rf time.raw tmp.lst
echo "Average: $average ms"
