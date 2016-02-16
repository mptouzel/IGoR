/*
 * Genechoice.h
 *
 *  Created on: Dec 9, 2014
 *      Author: quentin
 */

#ifndef GENECHOICE_H_
#define GENECHOICE_H_

#include "Rec_Event.h"
#include "Utils.h"
#include <forward_list>
#include <unordered_map>
#include <string>
#include <list>
#include <queue>
#include <utility>
#include "Errorrate.h"
#include <random>

class Gene_choice: public Rec_Event {
public:
	//Constructors
	Gene_choice();
	Gene_choice(Gene_class);
	Gene_choice(Gene_class ,std::unordered_map<std::string,Event_realization>&);
	Gene_choice(Gene_class,std::vector<std::pair<std::string,std::string>>);
	//Destructor
	virtual ~Gene_choice();
	//Virtual methods overload
	Rec_Event* copy();
	inline void iterate(double& , double& , const std::string& , const std::string& , Index_map& , const std::unordered_map<Rec_Event_name,std::vector<std::pair<const Rec_Event*,int>>>& , std::queue<Rec_Event*>& , Marginal_array_p& , const Marginal_array_p& , const std::unordered_map<Gene_class , std::vector<Alignment_data>>& , Seq_type_str_p_map& , Seq_offsets_map& ,Error_rate*&, const std::unordered_map<std::tuple<Event_type,Gene_class,Seq_side>, Rec_Event*>& , Safety_bool_map& , Mismatch_vectors_map& , double& , double&);
	bool add_realization(std::string gene_name , std::string gene_sequence  );
	std::queue<int> draw_random_realization(const Marginal_array_p , std::unordered_map<Rec_Event_name,int>& , const std::unordered_map<Rec_Event_name,std::vector<std::pair<const Rec_Event*,int>>>& , std::unordered_map<Seq_type , std::string>& , std::default_random_engine&)const;
	void write2txt(std::ofstream&);
	void initialize_event( std::unordered_set<Rec_Event_name>& , const std::unordered_map<std::tuple<Event_type,Gene_class,Seq_side>, Rec_Event*>& , const std::unordered_map<Rec_Event_name,std::vector<std::pair<const Rec_Event*,int>>>& , Seq_type_str_p_map& , Safety_bool_map& , Error_rate* ,Mismatch_vectors_map&,Seq_offsets_map&,Index_map&);
	void add_to_marginals(long double , Marginal_array_p) const;

private:
	inline double iterate_common(double ,const int&  , int , Index_map& ,const std::unordered_map<Rec_Event_name,std::vector<std::pair<const Rec_Event*,int>>>& ,const Marginal_array_p);


	//Inference variables
	//Bool checks
	bool vd_check;
	bool vj_check;
	bool dj_check;

	//Offsets checks


	Seq_Offset d_5_min_offset;
	Seq_Offset d_5_max_offset;
	Seq_Offset j_5_min_offset;
	Seq_Offset j_5_max_offset;
	Seq_Offset v_5_off;
	Seq_Offset v_3_off;
	Seq_Offset d_offset;
	Seq_Offset j_offset;
	Seq_Offset v_offset;




	Seq_Offset v_3_min_offset;
	Seq_Offset v_3_max_offset;
	Seq_Offset d_3_off;
	Seq_Offset d_5_off;



	Seq_Offset d_3_min_offset;
	Seq_Offset d_3_max_offset;


	Seq_Offset j_5_off;

	//Suitable D align bool
	bool no_d_align;
	std::vector<int> no_d_mismacthes;
	size_t d_size;
	Seq_Offset d_full_3_offset;



	//Declare common variables
	int base_index;
	double new_scenario_proba;
	double new_tmp_err_w_proba;
	double proba_contribution;
	std::string gene_seq;
	int new_index;

	//Constants
		//Memory Layers
		int memory_layer_cs;
		int memory_layer_mismatches;
		int memory_layer_safety_1;
		int memory_layer_safety_2;
		int memory_layer_off_threep;
		int memory_layer_off_fivep;
		int memory_layer_offset_check1;
		int memory_layer_offset_check2;

		//Gene choices
		bool v_chosen;
		bool v_choice_exist;
		bool d_chosen;
		bool d_choice_exist;
		bool j_chosen;
		bool j_choice_exist;

		//Deletion ranges
		int d_5_max_del;
		int d_5_min_del;
		int d_5_real_max_del;
		int j_5_max_del;
		int j_5_min_del;
		int v_3_max_del;
		int v_3_min_del;
		int d_3_max_del;
		int d_3_min_del;





};

#endif /* GENECHOICE_H_ */