/*
 * Aligner.h
 *
 *  Created on: Feb 16, 2015
 *      Author: quentin
 */

#ifndef ALIGNER_H_
#define ALIGNER_H_

#include<forward_list>
#include<list>
#include<unordered_map>
#include <utility>
#include <fstream>
#include <algorithm>
#include <iostream>
#include "Utils.h"
#include <omp.h>
#include <stdexcept>
#include <random>
#include <chrono>


template<typename T> struct Matrix {
public:
	Matrix():rows(0) , cols(0) , array_p(new T [0]){}
	Matrix(int m ,int n ): rows(m) , cols(n) , array_p(new T [m*n] ){}
	Matrix(int m , int n , T arr[]):rows(m) , cols(n) , array_p(new T [m*n]){
		for(size_t i = 0 ; i != m*n ; i++){
			array_p[i] = arr[i];
		}
	}
	Matrix(const Matrix<T>& other){
		//Provides deep copy of a matrix
		this->rows = other.rows;
		this->cols = other.cols;
		this->array_p = new T [rows*cols];
		for(int i = 0 ; i != rows*cols ; i++){
			this->array_p[i] = other.array_p[i];
		}
	}
	~Matrix(){delete [] array_p;}
	T& operator()(int i ,int j ){
		if( (i>rows-1) || (j>cols-1) ){
			throw std::length_error("Cannot access indices ["+std::to_string(i)+","+std::to_string(j)+"] with matrix dimensions ["+std::to_string(rows)+","+std::to_string(cols)+"]");
			std::cout<<"out_of range matrix coordinates: "<<rows<<"<"<<i<<" or "<<cols<<"<"<<j<<std::endl;
		}
		return array_p[i + rows*j];
	}
private:
	int rows;
	int cols;
	T* array_p ;
};


struct Alignment_data {
	std::string gene_name;
	int offset;
	std::forward_list<int> insertions; //gap in the genomic sequence
	std::forward_list<int> deletions; //gap in the data sequence
	size_t align_length;
	mutable std::vector<int> mismatches;
	double score;

	Alignment_data(std::string gene , int off): gene_name(gene) , offset(off) , insertions(*(new std::forward_list<int>)) , deletions(*(new std::forward_list<int>)) , score(0) {}
	Alignment_data(int off , size_t align_len , std::forward_list<int> ins , std::forward_list<int> del , std::vector<int> mis , double alignment_score): gene_name(std::string()) , offset(off) , insertions(ins) , deletions(del) , align_length(align_len) , mismatches(mis) , score(alignment_score) {}
	Alignment_data(std::string gene , int off , size_t align_len , std::forward_list<int> ins , std::forward_list<int> del , std::vector<int> mis , double alignment_score): gene_name(gene) , offset(off) , insertions(ins) , deletions(del) , align_length(align_len) , mismatches(mis) , score(alignment_score) {}

/*	bool operator<(const Alignment_data& align){
		//Hardcode to get the alignments in descending order using sort()
		return this->score > align.score;
	}*/

};

class Aligner {
public:
	Aligner();
	Aligner(Matrix<double>,int,Gene_class);
	virtual ~Aligner();

	std::forward_list<Alignment_data> align_seq(std::string , double , bool , int , int);
	std::unordered_map<int,std::forward_list<Alignment_data>> align_seqs(std::vector<std::pair<const int , const std::string>> , double , bool);
	std::unordered_map<int,std::forward_list<Alignment_data>> align_seqs(std::vector<std::pair<const int , const std::string>> , double , bool , int , int);
	void align_seqs( std::string , std::vector<std::pair<const int , const std::string>> , double , bool );
	void align_seqs( std::string , std::vector<std::pair<const int , const std::string>> , double , bool , int , int);
	void write_alignments_seq_csv(std::string , std::unordered_map<int,std::forward_list<Alignment_data>>);

	std::unordered_map<int,std::forward_list<Alignment_data>> read_alignments_seq_csv(std::string , double , bool);

	void set_genomic_sequences(std::vector< std::pair<std::string,std::string> >);
	int incorporate_in_dels( std::string& , std::string& , const std::forward_list<int> , const std::forward_list<int> , int );


private:
	std::forward_list<std::pair<std::string,std::string>> nt_genomic_sequences;
	std::forward_list<std::pair<std::string,std::string>> int_genomic_sequences;
	Matrix<double> substitution_matrix;
	int gap_penalty;
	int score_threshold;
	Gene_class gene;
	bool local_align;
	bool flip_seqs;
	void sw_align_common(const std::string& ,const std::string& ,const int,const int , Matrix<double>& , Matrix<int>& , Matrix<int>& , Matrix<int>& , std::vector<int>& ,  std::vector<int>& , std::vector<int>&);
	std::list<std::pair<int,Alignment_data>> sw_align(const std::string& ,const std::string& , double , bool , int , int);


};


std::unordered_map<int,std::pair<std::string,std::unordered_map<Gene_class,std::vector<Alignment_data>>>> read_alignments_seq_csv(std::string , Gene_class , double , bool , std::vector<std::pair<const int,const std::string>>);
std::unordered_map<int,std::pair<std::string,std::unordered_map<Gene_class,std::vector<Alignment_data>>>> read_alignments_seq_csv(std::string , Gene_class , double , bool , std::vector<std::pair<const int,const std::string>>, std::unordered_map<int,std::pair<std::string,std::unordered_map<Gene_class,std::vector<Alignment_data>>>>);
std::unordered_map<int,std::pair<std::string,std::unordered_map<Gene_class,std::vector<Alignment_data>>>> read_alignments_seq_csv_score_range(std::string , Gene_class , double , bool , std::vector<std::pair<const int,const std::string>>);
std::unordered_map<int,std::pair<std::string,std::unordered_map<Gene_class,std::vector<Alignment_data>>>> read_alignments_seq_csv_score_range(std::string , Gene_class , double , bool , std::vector<std::pair<const int,const std::string>>, std::unordered_map<int,std::pair<std::string,std::unordered_map<Gene_class,std::vector<Alignment_data>>>>);
std::vector<std::pair<std::string,std::unordered_map<Gene_class,std::vector<Alignment_data>>>> map2vect (std::unordered_map<int,std::pair<std::string,std::unordered_map<Gene_class,std::vector<Alignment_data>>>>);
std::forward_list<std::pair<const int,const std::string>> read_indexed_seq_csv(std::string);
std::vector<std::pair<const int , const std::string>> read_indexed_csv(std::string);
std::vector<std::pair<const int,const std::string>> read_fasta(std::string);
std::vector<std::pair<std::string,std::string>> read_genomic_fasta(std::string);
std::vector<std::pair<const int,const std::string>> read_txt(std::string);
void write_indexed_seq_csv(std::string , std::vector<std::pair<const int,const std::string>>);
std::string nt2int(std::string);
bool comp_nt_int(const char& , const char&);
inline void write_single_seq_alignment( std::ofstream& , int , std::forward_list<Alignment_data> );
//Compare alignments (sort by score)
bool align_compare(Alignment_data , Alignment_data );
std::vector<std::pair<const int , const std::string>> sample_indexed_seq( std::vector<std::pair<const int , const std::string>>,const size_t);

/*
	namespace substitution_matrices{
		//from: ftp://ftp.ncbi.nih.gov/blast/matrices/NUC.4.4
		static Matrix<int> nuc44_sub_matrix(4,4,{5,-4,-4,-4 , -4 ,5,-4,-4 , -4,-4,5,-4 , -4,-4,-4,5});


	}
	*/



#endif /* ALIGNER_H_ */