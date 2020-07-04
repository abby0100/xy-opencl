#!/bin/bash
source=$1
target=xy

	# compile
	g++ $source -o $target -lOpenCL

	# run
	./$target
