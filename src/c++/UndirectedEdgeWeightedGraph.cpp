#include "UndirectedEdgeWeightedGraph.h"

template <typename T>
UndirectedEdgeWeightedGraph<T>::UndirectedEdgeWeightedGraph() {}

template <typename T>
UndirectedEdgeWeightedGraph<T>::UndirectedEdgeWeightedGraph(int n_nodes) {
	/*Creates object with known number of nodes to be added*/

	adj.resize(n_nodes);
	for(unsigned int i = 0; i < adj.size(); i++)
		adj[i].resize(n_nodes);
}

template <typename T>
void UndirectedEdgeWeightedGraph<T>::add_node(T node) {
	/*Adds node with T identifier*/

	node_info_t my_node_info;
	my_node_info.index = nodes.size();
	my_node_info.visited = 0;
	nodes.insert(std::make_pair(node, my_node_info));

	if(adj.size() < nodes.size()){
		adj.resize(adj.size() + 1);
		adj[adj.size() - 1].resize(adj.size());
	}
}

template <typename T>
void UndirectedEdgeWeightedGraph<T>::add_edge(T node1, T node2, double weight) {
	/*Adds edge connecting two existing nodes with given weight*/

	int x = (nodes.at(node1)).index;
	int y = (nodes.at(node2)).index;
	adj[x][y] = weight;
	adj[y][x] = weight;
}

template <typename T>
void UndirectedEdgeWeightedGraph<T>::add_connected_nodes(T node1, T node2, double weight) {
	/*Adds two nodes and connects them with given weight*/
	node_info_t my_node_info1, my_node_info2;

	my_node_info1.index = nodes.size();
	my_node_info1.visited = 0;
	nodes.insert(std::make_pair(node1, my_node_info1));

	my_node_info2.index = nodes.size();
	my_node_info2.visited = 0;
	nodes.insert(std::make_pair(node2, my_node_info2));

	if(adj.size() < nodes.size()){
		adj.resize(adj.size() + 2);
		adj[adj.size() - 1].resize(adj.size());
		adj[adj.size() - 2].resize(adj.size());
	}

	adj[my_node_info1.index][my_node_info2.index] = weight;
	adj[my_node_info2.index][my_node_info1.index] = weight;
}

template <typename T>
int UndirectedEdgeWeightedGraph<T>::are_connected(T node1, T node2) {
	/*Returns 1 if given nodes are directly connected and 0 otherwise*/

	int x = (nodes.at(node1)).index;
	int y = (nodes.at(node2)).index;
	if (adj[x][y] > 0) return 1;
	return 0;
}

template <typename T>
double UndirectedEdgeWeightedGraph<T>::get_edge_weight(T node1, T node2) {
	/*Returns weight of the edge connecting two existing and directly connected nodes.
	 *If not connected, returns 0.0*/
	if (!are_connected(node1, node2)) return 0.0;
	int x = (nodes.at(node1)).index;
	int y = (nodes.at(node2)).index;
	return adj[x][y];
}

template <typename T>
int UndirectedEdgeWeightedGraph<T>::get_number_nodes() {
	return nodes.size();
}

template <typename T>
std::vector<std::vector<T> > UndirectedEdgeWeightedGraph<T>::connected_components(double weight) {
	/*Returns vector of connected components where each position is a vector of nodes in the
	/*same component*/

	std::vector<std::vector<T> > components;
	for(typename std::map<T, node_info_t>::iterator it = nodes.begin(); it != nodes.end(); ++it)
		(it->second).visited = 0;

	for(typename std::map<T, node_info_t>::iterator it = nodes.begin(); it != nodes.end(); ++it)
		if (!(it->second).visited)
			//For every unvisited node, runs DFS to get all connected nodes
			DFS_vector_fill(it, components, weight);
	return components;
}

template <typename T>
void UndirectedEdgeWeightedGraph<T>::DFS_vector_fill(typename std::map<T, node_info_t>::iterator n,
											   	  std::vector<std::vector<T> > &components,
												  double weight) {
	/*Runs DFS from a starting node "n" and adds a vector to components with all the nodes in the same
	/*connected component as n*/

	std::stack<typename std::map<T, node_info_t>::iterator> my_stack;
	std::vector<T> aux;
	my_stack.push(n);
	while (!my_stack.empty()) {
		n = my_stack.top();
		my_stack.pop();
		if (!(n->second).visited) {
			aux.push_back(n->first);
			(n->second).visited = 1;
		}
		for(typename std::map<T, node_info_t>::iterator it = nodes.begin(); it != nodes.end(); ++it)
			if (adj[(n->second).index][(it->second).index] >= weight && !(it->second).visited)
				my_stack.push(it);
	}
	components.push_back(aux);
}

//Template classes instantiation
template class UndirectedEdgeWeightedGraph<std::string>;
