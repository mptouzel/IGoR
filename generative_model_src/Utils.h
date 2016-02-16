/*
 * Utils.h
 *
 *  Created on: Apr 9, 2015
 *      Author: quentin
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <fstream>
#include <vector>
#include <string>
#include <utility>
#include <tuple>
#include <stdexcept>
#include <iostream>

enum Event_type {GeneChoice_t , Deletion_t , Insertion_t , Dinuclmarkov_t,Undefined_t};
enum Event_safety{VD_safe = 0  , DJ_safe = 1  , VJ_safe = 2 };
enum Seq_side{ Five_prime =0 , Three_prime = 1 , Undefined_side = 2 };
enum Seq_type {V_gene_seq = 0 , VD_ins_seq = 1 , D_gene_seq = 2 , DJ_ins_seq = 3 , J_gene_seq = 4 , VJ_ins_seq = 5};
enum Gene_class{V_gene , VD_genes , D_gene , DJ_genes , J_gene , VJ_genes , VDJ_genes ,Undefined_gene };

std::ostream& operator<<(std::ostream& , Gene_class);
std::ostream& operator<<(std::ostream& , Seq_side);
std::string operator+(const std::string& , Gene_class );
std::string operator+(const std::string& , Seq_side );
std::string operator+(const std::string& , Event_type );

//Type used to describe the array of doubles containing the marginals values
typedef long double* Marginal_array_p;

//Type used as key for unordered map since Rec_event cannot be instantiated
typedef std::string Rec_Event_name;

//Type used for offset of alignmed sequences in sequence_offsets maps. Used to characterize the beginning and the end of a sequence on the data sequence
typedef int Seq_Offset;
typedef std::string* Str_ptr;

/*
 * This class provides a fast alternative to unordered_map<Seq_type,string*> for the constructed_sequences objects
 * Change this and give some kind of matrix with memory levels
 * Create a 0 size at first?
 * Get rid of it in the deletions
 */
template<typename K, typename V> class  Enum_fast_memory_map {
public:
		Enum_fast_memory_map(int defined_range):max_layer(0) , range(defined_range){
			value_ptr_arr =  new V [range];
			memory_layer_ptr = new int [range];
			for(size_t i = 0 ; i!=range ; ++i){
				this->memory_layer_ptr[i] = -1;
			}
			/*for(size_t i = 0 ; i!=range ; i++){
				str_ptr_arr[i] = nullptr;
			}*/
		}
		virtual ~Enum_fast_memory_map(){
			delete [] value_ptr_arr;
			delete [] memory_layer_ptr;
		}

		//Accessors
		V& operator[](const K& key){
			if(key>range-1){throw std::out_of_range("Unknown key in Enum_fast_memory_map::operator(Seq_type)");}
			if(memory_layer_ptr[key]>-1){
				return (*(value_ptr_arr+ key + memory_layer_ptr[key]*range));
			}
			else{
				memory_layer_ptr[key] = 0;
				return (*(value_ptr_arr+key));
			}

		}


		V& at(const K& key){
			if(key>range-1){
				throw std::out_of_range("Unknown key in Enum_fast_memory_map::operator(K key)");
			}
			else{
				if(memory_layer_ptr[key]>-1){
					return (*(value_ptr_arr + key + memory_layer_ptr[key]*range));
				}
				else{
					throw std::out_of_range("Trying to access uninitialized position in Enum_fast_memory_map::at(const K& key)");
				}
			}
		}


		V& at(const K& key ,int memory_layer){
			if(key>range-1){
				throw std::out_of_range("Unknown seq type in Enum_fast_memory_map::operator(Seq_type)");
			}
			else{
				if(memory_layer<=(memory_layer_ptr[key]+1)){
					memory_layer_ptr[key] = memory_layer;
					return (*(value_ptr_arr + key + memory_layer*range));
				}
				else{
					throw std::out_of_range("Trying to access uninitialized position in Enum_fast_memory_map::at( const K& key, int memory_layer)");
				}
			}
		}


		int get_current_memory_layer(const K& key){
			return memory_layer_ptr[key];
		}

		bool exist(const K& key){
			return memory_layer_ptr[key]>-1;
		}


		void request_memory_layer(const K& key){
			/*std::cout<<key<<std::endl;
			std::cout<<memory_layer_ptr[key]<<std::endl;*/
			if(key>range-1){
				throw std::out_of_range("Unknown key in Enum_fast_memory_map::request_memory_layer()");
			}
			//Get current memory layer at this position
				if(memory_layer_ptr[key]<max_layer){
					++memory_layer_ptr[key];
				}
				else{
					++max_layer;
					V* new_value_ptr = new V [range*(max_layer+1)];
					for(size_t i = 0 ; i!=range ; ++i){
						for(size_t j=0 ; j!=(max_layer+1) ; ++j){
							(*(new_value_ptr + i+j*range)) = (*(value_ptr_arr + i+j*range));
						}
					}
					delete [] value_ptr_arr;
					value_ptr_arr = new_value_ptr;
					++memory_layer_ptr[key];
				}
			}


		//Setters
		void set_value(const K& key ,const V& value , int memory_layer){
			if(key>range-1){throw std::out_of_range("Unknown seq type in Seq_type_str_p_map::operator(Seq_type)");}
				//Cannot fill memory layer without filling the ones downstream
				if(memory_layer<=(memory_layer_ptr[key]+1)){
					(*(value_ptr_arr + key + memory_layer*range)) = value;
					//Setting a value at a given layer invalidate upper layers
					memory_layer_ptr[key] = memory_layer;
				}
				else{
					throw std::out_of_range("Trying to access incorrect memory layer in Enum_fast_memory_map::set_value()");
				}
		}


protected:
	V* value_ptr_arr;
	int* memory_layer_ptr;
	int max_layer;
	size_t range; //= 6; //number of outcomes in Seq_type

};



typedef Enum_fast_memory_map<Seq_type,Str_ptr> Seq_type_str_p_map;

typedef Enum_fast_memory_map<Event_safety,bool> Safety_bool_map;

typedef Enum_fast_memory_map<Seq_type,std::vector<int>*> Mismatch_vectors_map;

typedef Enum_fast_memory_map<int,size_t> Index_map;

/*
	template<> class Enum_fast_memory_map<Seq_type ,Str_ptr>{
	Enum_fast_memory_map():Enum_fast_memory_map<Seq_type,Str_ptr>(6){};
	size_t range = 6;
};
*/


/*template<>
class Seq_type_str_p_map : public Enum_fast_memory_map<Seq_type,Str_ptr>{

};*/


/*
 * This class provides a fast alternative to unordered_map<Seq_type,string*> for the constructed_sequences objects
 * Change this and give some kind of matrix with memory levels
 * Create a 0 size at first?
 * Get rid of it in the deletions
 */
template<typename K1, typename K2 , typename V> class  Enum_fast_memory_dual_key_map {
public:
		Enum_fast_memory_dual_key_map(size_t Key1_range , size_t Key2_range):max_layer(0) , range_key1(Key1_range) , range_key2(Key2_range){
			total_range = range_key1*range_key2;
			value_ptr_arr =  new V [total_range];
			memory_layer_ptr = new int [total_range];
			for(size_t i = 0 ; i!=total_range ; ++i){
				this->memory_layer_ptr[i] = -1;
			}
			/*for(size_t i = 0 ; i!=range ; i++){
				str_ptr_arr[i] = nullptr;
			}*/
		}
		virtual ~Enum_fast_memory_dual_key_map(){
			delete [] value_ptr_arr;
			delete [] memory_layer_ptr;
		}

		//Accessors
		//Cannot use [] with more than one argument
		/*V& operator[](const K1& key1 , const K2& key2){
			if(key1>range_key1-1){throw std::out_of_range("Unknown key1 in Enum_fast_memory_map::operator(Seq_type)");}
			if(key2>range_key2-1){throw std::out_of_range("Unknown key2 in Enum_fast_memory_map::operator(Seq_type)");}
			if(memory_layer_ptr[key1+range_key1*key2]>-1){
				return (*(value_ptr_arr+ key1+range_key1*key2 + memory_layer_ptr[key1+range_key1*key2]*total_range));
			}
			else{
				memory_layer_ptr[key1+range_key1*key2] = 0;
				return (*(value_ptr_arr+key1+range_key1*key2));
			}

		}*/


		V& at(const K1& key1 , const K2& key2){
			if(key1>range_key1-1){
				throw std::out_of_range("Unknown key1 in Enum_fast_memory_dual_key_map::at()");
			}
			else if(key2>range_key2-1){
				throw std::out_of_range("Unknown key2 in Enum_fast_memory_dual_key__map::at()");
			}
			else{
				if(memory_layer_ptr[key1+range_key1*key2]>-1){
					return (*(value_ptr_arr + key1+range_key1*key2 + memory_layer_ptr[key1+range_key1*key2]*total_range));
				}
				else{
					throw std::out_of_range("Trying to access uninitialized position in Enum_fast_memory_dual_key__map::at()");
				}
			}
		}


		V& at(const K1& key1 , const K2& key2 , int memory_layer){
			if(key1>range_key1-1){
				throw std::out_of_range("Unknown key1 in Enum_fast_memory_dual_key__map::at()");
			}
			else if(key2>range_key2-1){
				throw std::out_of_range("Unknown key2 in Enum_fast_memory_dual_key__map::at()");
			}
			else{
				if(memory_layer<=(memory_layer_ptr[key1+range_key1*key2]+1)){
					memory_layer_ptr[key1+range_key1*key2] = memory_layer;
					return (*(value_ptr_arr + key1+range_key1*key2 + memory_layer*total_range));
				}
				else{
					throw std::out_of_range("Trying to access uninitialized position in Enum_fast_memory_dual_key__map::at()");
				}
			}
		}


		int get_current_memory_layer(const K1& key1 , const K2& key2){
			return memory_layer_ptr[key1+range_key1*key2];
		}


		void request_memory_layer(const K1& key1 , const K2& key2){
			/*std::cout<<key<<std::endl;
			std::cout<<memory_layer_ptr[key]<<std::endl;*/
			if(key1>range_key1-1){
				throw std::out_of_range("Unknown key1 in Enum_fast_memory_dual_key__map::request_memory_layer()");
			}
			if(key2>range_key2-1){
				throw std::out_of_range("Unknown key2 in Enum_fast_memory_dual_key__map::request_memory_layer()");
			}
			//Get current memory layer at this position
				if(memory_layer_ptr[key1+range_key1*key2]<max_layer){
					++memory_layer_ptr[key1+range_key1*key2];
				}
				else{
					++max_layer;
					V* new_value_ptr = new V [total_range*(max_layer+1)];
					for(size_t i = 0 ; i!=total_range ; ++i){
						for(size_t j=0 ; j!=(max_layer+1) ; ++j){
							(*(new_value_ptr + i+j*total_range)) = (*(value_ptr_arr + i+j*total_range));
						}
					}
					delete [] value_ptr_arr;
					value_ptr_arr = new_value_ptr;
					++memory_layer_ptr[key1+range_key1*key2];
				}
			}


		//Setters
		void set_value(const K1& key1 , const K2& key2 ,V value , int memory_layer){
			if(key1>range_key1-1){throw std::out_of_range("Unknown key1 in Seq_type_str_p_map::set_value()");}
			if(key2>range_key2-1){throw std::out_of_range("Unknown key2 in Seq_type_str_p_map::set_value()");}
				//Cannot fill memory layer without filling the ones downstream
				if(memory_layer<=(memory_layer_ptr[key1+range_key1*key2]+1)){
					(*(value_ptr_arr + key1+range_key1*key2 + memory_layer*total_range)) = value;
					//Setting a value at a given layer invalidate upper layers
					memory_layer_ptr[key1+range_key1*key2] = memory_layer;
				}
				else{
					throw std::out_of_range("Trying to access incorrect memory layer in Enum_fast_memory_dual_key__map::set_value()");
				}
		}


protected:
	V* value_ptr_arr;
	int* memory_layer_ptr;
	int max_layer;
	size_t range_key1; //= 6; //number of outcomes in Seq_type
	size_t range_key2;
	size_t total_range;

};

typedef Enum_fast_memory_dual_key_map<Seq_type,Seq_side,Seq_Offset> Seq_offsets_map;







/*
 * Defining a hash functions for Rec_Event, Gene_class and pair<Gene_class,Seq_side>
 */
 namespace std{
	 /*
 	 template<>
	 struct hash<Rec_Event>{
		inline std::size_t operator()(const Rec_Event& event) const{ //TODO inline?
			return  (((hash<int>()(event.get_class())
					^(hash<int>()(event.get_side())<<1 )) >>1)
					^(hash<int>()(event.get_priority())<<1)>>1)
					^(hash<int>()(event.get_realizations_map().size())<<1);
			//Note : only consider the size of the realization map and not what it contains for speed purposes
			//this should be enough to ensure no collisions
		}
	 };
	 */


/*
	 template<>
	 struct hash<Rec_Event*>{
		 std::size_t operator()(const Rec_Event*& event_point) const{
			 return hash<Rec_Event>()(*event_point);
		 }
	 };
	 */

	 template<>
	 	 struct hash<Seq_type>{
	 		 std::size_t operator()(const Seq_type& seq_t) const{
	 		 			return  hash<int>()(seq_t);
	 		 		}
	 	 };

	template<>
		 struct hash<Gene_class>{
			 std::size_t operator()(const Gene_class& gene) const{
						return  hash<int>()(gene);
					}
		 };

	 template<>
	 struct hash<std::pair<Gene_class,Seq_side>>{
		 std::size_t operator()(const pair<Gene_class,Seq_side>& gene_pair) const{
		 		 		return  (hash<Gene_class>()(gene_pair.first)
		 		 				^(hash<int>()(gene_pair.second) <<1))>>1;
		 		 	}
	 };

	 template<>
	 struct hash<std::tuple<Event_type,Gene_class,Seq_side>>{
		std::size_t operator()(const std::tuple<Event_type,Gene_class,Seq_side>& event_triplet) const{
			Event_type ev_type;
			Gene_class g_class;
			Seq_side s_side;
			std::tie(ev_type,g_class,s_side) = event_triplet;
			return ((hash<int>()(ev_type)
					^(hash<int>()(g_class) <<1)>>1)
					^(hash<int>()(s_side) <<1));

		}
	 };

	 template<>
	 struct hash<std::pair<Seq_type,Seq_side>>{
		std::size_t operator()(const std::pair<Seq_type,Seq_side> seq_pair) const{
			return  (hash<int>()(seq_pair.first)
					 		 				^(hash<int>()(seq_pair.second) <<1))>>1;
		}
	 };

	 template<>
	 struct hash<Event_safety>{
		 std::size_t operator()(const Event_safety ev_saf) const{
			 return (hash<int>()(ev_saf));
		 }
	 };
 }




#endif /* UTILS_H_ */