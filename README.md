# FlowCutter PACE 2016 Submission

This repository contains the FlowCutter code submitted to the [PACE 2016](https://pacechallenge.wordpress.com/track-a-treewidth/) tree decomposition challenge. 
If you are running a Unix-like system, then getting started is very simple. Just clone the repository and build the programs, as follows:

```bash
git clone flow_cutter_pace16
cd flow_cutter_pace16
./build.sh
```

There are no dependencies beyond a GCC with version 4.8 or newer. Clang should also work, but has not been tested by us.

After executing the build script, the root directory of the repository should contain the two binary files `flow_cutter_pace16` and `flow_cutter_parallel_pace16`. These are the programs entered into the heuristic, sequential and heuristic, parallel tracks of the competition. The outputted decompositions are guaranteed to be valid but do not necessarily have a minimum width. Both executable have the same interface. 

There are three ways to correctly invoke the program:

```bash
./flow_cutter_pace16 < my_graph.gr 
./flow_cutter_pace16 my_graph.gr
./flow_cutter_pace16 -s 42 < my_graph.gr
```

The first and the last commands read the input graph from the standard input. The second command reads it from a file whose name is given as parameter. The `-s` parameter sets the random seed. By default a seed of 0 is assumed. We tried to make sure that given the same seed, the behaviour of the sequential binary should be the identical even accross compilers.

The executables run until either a SIGINT or SIGTERM signal is sent. Once this signal is encountered the programm prints a tree decomposition to the standard output with the smallest width that it could found and terminates. Note that the output is empty if you send the signal before any decomposition is found.

The format specification of the input graph and output decompositions follow those of the [PACE 2016](https://pacechallenge.wordpress.com/track-a-treewidth/) challenge. 

The parallization is done using OpenMP. You can therefore control the number of threads spawned by setting the environment variable `OMP_NUM_THREADS` and use any of the other [OpenMP environment variables](https://gcc.gnu.org/onlinedocs/libgomp/Environment-Variables.html) used in the GCC's OpenMP implementation.

## Publications

Please cite the following article if you use our code in a publication:

* Graph Bisection with Pareto-Optimization.
  Michael Hamann and Ben Strasser.
  Proceedings of the 18th Meeting on Algorithm Engineering and Experiments (ALENEX'16).

