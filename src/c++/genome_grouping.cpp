#include "genome_grouping.h"

static std::vector<GenomicNeighborhood> parse_neighborhoods(std::string neighborhoods_file) {
    /*Receives a file generated by parse_neighborhood.py.
    /*Returns a vector of genomic neighborhoods, filled with the information from the file.*/

    std::ifstream file;
    std::string token;
    std::string accession;
    std::string organism;
    std::string locus;
    std::string pid;
    std::string cds;
    std::vector<GenomicNeighborhood> neighborhoods;

    file.open(neighborhoods_file.c_str());
        while(std::getline(file, token)) {
            //token is already the genus
            organism = token + ' ';
            std::getline(file, token); //species
            organism += token;
            std::getline(file, token); //accession
            accession = token;

            GenomicNeighborhood temp_neigh(accession, organism);

            while (std::getline(file, token) && token != "end") {
                if (token == "seed") {
                    std::getline(file, token); //locus from seed
                    locus = token;
                    std::getline(file, token); //pid from seed
                    pid = token;
                    std::getline(file, token); //cds from seed
                    cds = token;
                    temp_neigh.add_protein(locus, pid, cds);
                    temp_neigh.set_seed(locus, pid, cds);
                }
                else {
                    //token is already the locus
                    locus = token;
                    std::getline(file, token); //pid
                    pid = token;
                    std::getline(file, token); //cds
                    cds = token;
                    temp_neigh.add_protein(locus, pid, cds);
                }
            }
            neighborhoods.push_back(temp_neigh);
        }
        file.close();
    return neighborhoods;
}
static int clustering_value(protein_info_t my_prot, protein_info_t my_prot2,
               UndirectedEdgeWeightedGraph<std::string> &clusters, double stringency) {
  /*Receives two proteins, a graph and a threshold value.
   *If the proteins are connected in the graph, return an integer with 100x their edge weight (because
    the Hungarian class only works with integers).
   *If the proteins aren't connected or if their edge weight is smaller than the threshold value, return 0.*/
  double edge_value = clusters.get_edge_weight(my_prot.pid, my_prot2.pid);
  if (edge_value >= stringency)
    return (int)(100*edge_value);
  else
    return 0;

}

static std::vector<std::vector<int> > fill_assignment_matrix(GenomicNeighborhood g1, GenomicNeighborhood g2,
                                                     UndirectedEdgeWeightedGraph<std::string> &clusters,
                                                     double stringency) {
    /*Receives two genomic neighborhoods, g1 and g2, and the similarity graph.
     *Fills an integer matrix where matrix[i][j] is the similarity measure between the i-th protein of g1 and
     *the j-th protein of g2 are in the same cluster (connected in the similarity graph) and 0 otherwise.*/

    int i = 0;
    int j = 0;
    std::vector<std::vector<int> > matrix;

    matrix.resize(g1.protein_count());
    for (unsigned int k = 0; k < matrix.size(); k++)
        matrix[k].resize(g2.protein_count());

    for(GenomicNeighborhood::iterator it = g1.begin(); it != g1.end(); ++it) {
        j = 0;
        for(GenomicNeighborhood::iterator it2 = g2.begin(); it2 != g2.end(); ++it2) {
            matrix[i][j] = clustering_value(*it, *it2, clusters, stringency);
	        //DEBUG
            //std::cout <<"matrix: " << i << " " << j << " " << it->pid << " " << it2->pid << " score = " << matrix[i][j]<<"\n";
            j++;
        }
        i++;
    }
    return matrix;
}

static double compare_neighborhoods(GenomicNeighborhood g1, GenomicNeighborhood g2,
                             UndirectedEdgeWeightedGraph<std::string> &clusters, double stringency) {
    /*Receives two genomic neighborhoods and a protein similarity graph.
     *Returns the MWM porthodom score between the two neighborhoods
     *(Using the hungarian algorithm and the porthodom scoring formula).*/

    double sum_temp = 0;
    std::map<std::pair<int, int>, int> assignments;
    std::vector<std::vector<int> > matrix = fill_assignment_matrix(g1, g2, clusters, stringency);
    Hungarian my_hungarian (matrix, matrix.size(), matrix[0].size(), HUNGARIAN_MODE_MAXIMIZE_UTIL);

    my_hungarian.solve();
    assignments = my_hungarian.get_assignments();
    //DEBUG
    /*std::cout <<"Assignments between (" << g1.get_accession() << ", " << g1.get_organism() << ") and "
    << "(" << g2.get_accession() << ", " << g2.get_organism() << "):\n";
    my_hungarian.print_assignment();
    my_hungarian.print_cost();
    fprintf(stderr, "\n");*/
    //PORTHODOM similarity measure calculation, adapted for genome neighborhoods
    //The similarity between to neighborhoods is given by the normalized sum of the
    //similarities between each pair of "assigned" proteins (as given by the Hungarian algorithm)
    //TO compute:
    //    1. add the assignment values
    //    2. divide by the number of proteins in the "largest" genome
    for (std::map<std::pair<int, int>,int>::iterator it = assignments.begin(); it != assignments.end(); ++it)
        sum_temp += ((double)it->second)/100; //Division to undo the multiplication in clustering_value()
        //DEBUG
        /*std::cout << "SUM_TEMP: " << sum_temp << "\n";*/
    return sum_temp/std::max(g1.protein_count(), g2.protein_count());
}

void genome_clustering(std::string neighborhoods_file, UndirectedEdgeWeightedGraph<std::string> &clusters,
                       std::string method, double stringency, std::string genome_sim_filename) {
    /*Receives a file containing all the genomic neighborhoods (as output by parse_neighborhood.py),
     *a protein similarity graph and the desired genomic neighborhood clustering method.
     *Writes the similarity between all genomic neighborhoods on the genome_sim_filename in
     *the format "organism1 acession1 organism2 acession2 score"*/
    /*INCOMPLETE*/
    std::ofstream file(genome_sim_filename.c_str());
    std::cout.precision(2); //configure output limiting to 2 decimal points
    std::vector<GenomicNeighborhood> neighborhoods = parse_neighborhoods(neighborhoods_file);
    double score;

    //DEBUG print the genomic neighborhoods we are considering
    /*for (int i = 0; i < neighborhoods.size(); i++) {
        std::cout << neighborhoods[i].get_accession() << " " << neighborhoods[i].get_organism() <<
                     " " << neighborhoods[i].protein_count() << "\n";
        for (GenomicNeighborhood::iterator it = neighborhoods[i].begin(); it != neighborhoods[i].end(); ++it) {
            std::cout << it->pid << " " << it->locus << " " << it->cds << "\n";
        }
    }
    std::cout << "\n"*/;
    if(neighborhoods.size() == 0) std::cout << "WHAAAT";
    if (method == "simple") {
        for(unsigned int m = 0; m < neighborhoods.size(); m++) {
            for (unsigned int n = m; n < neighborhoods.size(); n++) {
                score = compare_neighborhoods(neighborhoods[m], neighborhoods[n], clusters, stringency);
                file << neighborhoods[m].get_organism() << " " <<
                        neighborhoods[m].get_accession() << " " <<
                        neighborhoods[n].get_organism() << " " <<
                        neighborhoods[n].get_accession() << " " <<
                        score << "\n";
            }
        }
    }
}
