#!/bin/bash
important="$(hostname)"
for i in $(seq 1 $1)
do 
echo "Creating .jpg photo number: $i with size $2 x $3 pixels" 
./image_generator $2 $3
a="${i}.jpg"
#echo $a
mv example.jpg `printf %06d.%s ${a%.*} ${important}.${a##*.}`
done
