#ifndef SEPARATOR_H
#define SEPARATOR_H

#include "node_flow_cutter.h"
#include "flow_cutter.h"
#include "flow_cutter_config.h"
#include "union_find.h"
#include "tiny_id_func.h"
#include "min_max.h"

namespace flow_cutter{

	class ComputeSeparator{
	public:
		explicit ComputeSeparator(Config config):config(config){}

		template<class Tail, class Head, class InputNodeID>
		std::vector<int> operator()(const Tail&tail, const Head&head, const InputNodeID&)const{

			const int node_count = tail.image_count();
			const int arc_count = tail.preimage_count();

			auto out_arc = invert_sorted_id_id_func(tail);
			auto back_arc = compute_back_arc_permutation(tail, head);

			std::vector<int>separator;

			switch(config.separator_selection){
				case Config::SeparatorSelection::node_min_expansion:
				{
					
					auto expanded_graph = expanded_graph::make_graph(
						make_const_ref_id_id_func(tail), 
						make_const_ref_id_id_func(head), 
						make_const_ref_id_id_func(back_arc), 
						make_const_ref_id_func(out_arc)
					);

					auto cutter = make_simple_cutter(expanded_graph, config);
					auto pairs = select_random_source_target_pairs(node_count, config.cutter_count, config.random_seed);

					double best_score = std::numeric_limits<double>::max();

					cutter.init(expanded_graph::expand_source_target_pair_list(pairs), config.random_seed);
					for(;;){

						double cut_size = cutter.get_current_cut().size();
						double small_side_size = cutter.get_current_smaller_cut_side_size();

						double score = cut_size / small_side_size;

						if(cutter.get_current_smaller_cut_side_size() < config.min_small_side_size * expanded_graph::expanded_node_count(node_count))
							score += 1000000;
						

						if(score < best_score){
							best_score = score;
							separator = expanded_graph::extract_original_separator(tail, head, cutter).sep;
						}

						double potential_best_next_score = (double)(cut_size+1)/(double)(expanded_graph::expanded_node_count(node_count)/2);
						if(potential_best_next_score >= best_score)
							break;
						
						if(!cutter.advance())
							break;
						
					}
				}
				break;
				case Config::SeparatorSelection::edge_min_expansion:
				{
					auto graph = flow_cutter::make_graph(
						make_const_ref_id_id_func(tail), 
						make_const_ref_id_id_func(head), 
						make_const_ref_id_id_func(back_arc), 
						ConstIntIDFunc<1>(arc_count),
						make_const_ref_id_func(out_arc)
					);

					auto cutter = make_simple_cutter(graph, config);

					std::vector<int>best_cut;
					double best_score = std::numeric_limits<double>::max();

					cutter.init(select_random_source_target_pairs(node_count, config.cutter_count, config.random_seed), config.random_seed);

					for(;;){

						double cut_size = cutter.get_current_cut().size();
						double small_side_size = cutter.get_current_smaller_cut_side_size();

						double score = cut_size / small_side_size;

						if(cutter.get_current_smaller_cut_side_size() < config.min_small_side_size * node_count)
							score += 1000000;
						

						if(score < best_score){
							best_score = score;
							best_cut = cutter.get_current_cut();
						}

						double potential_best_next_score = (double)(cut_size+1)/(double)(expanded_graph::expanded_node_count(node_count)/2);
						if(potential_best_next_score >= best_score)
							break;
						
						if(!cutter.advance())
							break;
						
					}

					for(auto x:best_cut)
						separator.push_back(head(x));

					std::sort(separator.begin(), separator.end());
					separator.erase(std::unique(separator.begin(), separator.end()), separator.end());
				}
				break;
				case Config::SeparatorSelection::edge_first:
				{
					auto graph = flow_cutter::make_graph(
						make_const_ref_id_id_func(tail), 
						make_const_ref_id_id_func(head), 
						make_const_ref_id_id_func(back_arc), 
						ConstIntIDFunc<1>(arc_count),
						make_const_ref_id_func(out_arc)
					);

					auto cutter = make_simple_cutter(graph, config);
					cutter.init(select_random_source_target_pairs(node_count, config.cutter_count, config.random_seed), config.random_seed);
					while(cutter.get_current_smaller_cut_side_size() < config.min_small_side_size * node_count)
						if(!cutter.advance())
							break;

					for(auto x:cutter.get_current_cut())
						separator.push_back(head(x));

					std::sort(separator.begin(), separator.end());
					separator.erase(std::unique(separator.begin(), separator.end()), separator.end());
				}
				break;
				case Config::SeparatorSelection::node_first:
				{
					auto expanded_graph = expanded_graph::make_graph(
						make_const_ref_id_id_func(tail), 
						make_const_ref_id_id_func(head), 
						make_const_ref_id_id_func(back_arc), 
						make_const_ref_id_func(out_arc)
					);

					auto cutter = make_simple_cutter(expanded_graph, config);
					auto pairs = select_random_source_target_pairs(node_count, config.cutter_count, config.random_seed);

					cutter.init(expanded_graph::expand_source_target_pair_list(pairs), config.random_seed);
					while(cutter.get_current_smaller_cut_side_size() < config.min_small_side_size * expanded_graph::expanded_node_count(node_count))
						if(!cutter.advance())
							break;

					separator = expanded_graph::extract_original_separator(tail, head, cutter).sep;
				}
				break;
				default:
					assert(false);
					return {};
			}

			return std::move(separator);

		}
	private:
		Config config;
	};
}

#endif

