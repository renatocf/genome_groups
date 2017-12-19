#ifndef __GENOME_GROUPING_H__
#define __GENOME_GROUPING_H__

#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include "Hungarian.h"
#include "GenomicNeighborhood.h"
#include "UndirectedEdgeWeightedGraph.h"

/*Receives a file containing all the genomic neighborhoods (as output by parse_neighborhood.py),
 *a protein similarity graph and the desired genomic neighborhood clustering method.
 *Writes the similarity between all genomic neighborhoods on the genome_sim_filename
 *the format "organism1 acession1 coordinates1 organism2 acession2 coordinates2 score"*/
/*INCOMPLETE*/
void genome_clustering(std::string neighborhoods_filename, UndirectedEdgeWeightedGraph<std::string> &clusters,
                       std::string method, double stringency, std::string genome_sim_filename);

#endif
