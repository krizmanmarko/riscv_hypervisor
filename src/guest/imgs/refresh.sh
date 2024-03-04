#!/bin/bash

rm *.bin

for i in $(find .. -name '*.bin')
do
	cp $i .
done
