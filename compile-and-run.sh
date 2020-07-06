#!/bin/bash
source=$1
target=xy

	# remove
	rm $target

	# compile
	g++ $source -o $target -lOpenCL

	# run
	./$target
