
#include "id_func.h"
#include "list_graph.h"
#include "multi_arc.h"
#include "sort_arc.h"
#include "chain.h"
#include "flow_cutter.h"
#include "greedy_order.h"

#include "node_flow_cutter.h"
#include "contraction_graph.h"
#include "cch_order.h"
#include "tree_decomposition.h"
#include "separator.h"

#include <limits>
#include <iostream>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#ifdef PARALLELIZE
#include <omp.h>
#endif
using namespace std;

ArrayIDIDFunc tail, head;
ArrayIDIDFunc best_order;
int best_bag_size = numeric_limits<int>::max();

int compute_max_bag_size(const ArrayIDIDFunc&order){
	auto inv_order = inverse_permutation(order);
	int current_tail = -1;
	int current_tail_up_deg = 0;
	int max_up_deg = 0;
	compute_chordal_supergraph(
		chain(tail, inv_order), chain(head, inv_order), 
		[&](int x, int y){
			if(current_tail != x){
				current_tail = x;
				max_to(max_up_deg, current_tail_up_deg);
				current_tail_up_deg = 0;
			}
			++current_tail_up_deg;
		}
	);
	return max_up_deg+1;
}

unsigned long long get_milli_time(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (unsigned long long)(tv.tv_sec) * 1000
	   + (unsigned long long)(tv.tv_usec) / 1000;
}

void test_new_order(ArrayIDIDFunc order){
	int x = compute_max_bag_size(order);
	#ifdef PARALLELIZE
	#pragma omp critical
	#endif
	{
		if(x < best_bag_size){
			best_bag_size = x;
			best_order = order;
			cout << "c status " << best_bag_size << " " << get_milli_time() << endl;
		}
	}
}

void signal_handler(int)
{
	#ifdef PARALLELIZE
	#pragma omp critical
	#endif
	{
		if(tail.image_count() != 0 && best_order.image_count() != 0)
			print_tree_decompostion("-", tail, head, best_order);
	}
	exit(0);
}

int main(int argc, char*argv[]){
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);

	signal(SIGSEGV, signal_handler);
	try{
		{
			string file_name = "-";
			if(argc == 2)
				file_name = argv[1];
			auto g = uncached_load_pace_graph(file_name);
			tail = std::move(g.tail);
			head = std::move(g.head);
		}

		test_new_order(identity_permutation(tail.image_count()));
		test_new_order(compute_greedy_min_degree_order(tail, head));
		test_new_order(compute_greedy_min_shortcut_order(tail, head));

		int random_seed = 0;

		if(argc == 3){
			if(string(argv[1]) == "-s"){
				random_seed = atoi(argv[2]);
			}
		}

		#ifdef PARALLELIZE
		#pragma omp parallel
		#endif
		{
			try{
				std::minstd_rand rand_gen;
				rand_gen.seed(
					random_seed 
					#ifdef PARALLELIZE
					+ omp_get_thread_num()
					#endif
				);

				flow_cutter::Config config;
				config.cutter_count = 1;
				config.random_seed = rand_gen();

				for(int i=0;;++i){
					config.random_seed = rand_gen();
					if(i % 32 == 0)
						++config.cutter_count;
			
					test_new_order(cch_order::compute_cch_graph_order(tail, head, flow_cutter::ComputeSeparator(config)));
				}
			}catch(...){
				signal_handler(0);
			}
		}
	}catch(...){
		signal_handler(0);
	}
}

