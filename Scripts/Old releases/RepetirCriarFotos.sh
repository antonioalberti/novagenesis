#!/bin/bash
while :
do
        python CriarFotos.py different 100 &> /dev/null
        sleep $((10))
done
