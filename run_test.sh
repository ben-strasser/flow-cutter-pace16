#!/bin/sh
EXPECTED_MAX_BAG_SIZE=7
SEQ_MAX_BAG_SIZE=`timeout --signal=SIGTERM 10s ./flow_cutter_pace16 test/FolkmanGraph.gr | grep -E "^s td" | sed -E "s/^s td [0-9]+ ([0-9]+) [0-9]+$/\1/"`
PARALLEL_MAX_BAG_SIZE=`timeout --signal=SIGTERM 10s ./flow_cutter_parallel_pace16 test/FolkmanGraph.gr | grep -E "^s td" | sed -E "s/^s td [0-9]+ ([0-9]+) [0-9]+$/\1/"`

if [ "$EXPECTED_MAX_BAG_SIZE" = "$SEQ_MAX_BAG_SIZE" ]; then 
	echo "Sequential test passed"
else
	echo "Sequential test failed"
fi

if [ "$EXPECTED_MAX_BAG_SIZE" = "$PARALLEL_MAX_BAG_SIZE" ]
then 
	echo "Parallel test passed"
else
	echo "Parallel test failed"
fi
