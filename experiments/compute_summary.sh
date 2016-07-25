#!/bin/bash
(
	echo "graph,max_bag_size"
	for G in computed_decompositions/*
	do
		F=`basename $G`
		echo -n ${F%.td}
		echo -n ,
		grep -E "^s td" $G | sed -E "s/^s td ([0-9]+) ([0-9]+) ([0-9]+)$/\2/"
	done
) > summary.csv
