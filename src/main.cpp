#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string.h>
#include <algorithm>
#define DEBUG 0
#include "cpu-implementation.cpp"

//These are global for now. This will change when the implementation finishes.
std::vector<std::vector<int> > matrixOfEdges;
std::vector<std::pair<int,int> > *edges;
int numberOfVertices;

void find_result_gpu(std::vector<int>& adj, std::vector<int>& xadj, std::vector<int>& values, int size, int cycle_length);

void check_for_duplicates()
{
	//Some of the input files include both a-b and b-a for same edge.
	//Some of them does not have duplicates. To overcome this duality
	//We are assuming there are no repeating (only a-b is available in the input)
	//So we create b-a and then we check if there are any duplicates in the list.

 
	edges->erase(std::remove_if(edges->begin(),edges->end(), [] (std::pair<int,int> &p) {return p.first == p.second;}),edges->end());
	std::sort(edges->begin(), edges->end());
	edges->erase(std::unique(edges->begin(), edges->end()), edges->end());
	#if DEBUG
	for(int i = 0; i<edges->size(); i++)
	{
		std::cout<<edges->at(i).first<<" "<<edges->at(i).second<<std::endl;
	}
	#endif
}

std::vector<std::vector<int> > create_matrix_from_edges()
{
	//Matrix representation of the graph (duplicate free), this is used only for initial debug. At the final software this is not used. CSR from the input is directly created. This can be used for debug.  

	std::vector<std::vector<int> > adjacencyMatrix(numberOfVertices, std::vector<int>(numberOfVertices,0));

	for(int i = 0; i< edges->size(); i++)
	{
		std::cout<<i<< std::endl;
		adjacencyMatrix[edges->at(i).first][edges->at(i).second] = 1;
		adjacencyMatrix[edges->at(i).second][edges->at(i).first] = 1;
	}
	#if DEBUG
	for(int i = 0; i < numberOfVertices+1; i++)
	{
		for(int j = 0; j<numberOfVertices+1; j++)
		{
			printf("%d ", adjacencyMatrix[i][j]);
		}
		printf("\n");
	}
	#endif
	return adjacencyMatrix;
}

void create_csr_representation(std::vector<int> &adj, std::vector<int> &xadj)
{
	//CSR from input file is created without creating whole matrix.
	std::vector<int> numberOfElementsPerRow(numberOfVertices, 0);
	for(int i = 0; i<edges->size(); i++)
	{
		int currentRow = edges->at(i).first;
		int currentCol = edges->at(i).second;
		numberOfElementsPerRow[currentRow]++;
		adj.push_back(currentCol);
	}
	xadj[0] = 0;
	for(int i = 1; i<numberOfElementsPerRow.size(); i++)
	{
		xadj[i] += numberOfElementsPerRow[i-1];
		xadj[i+1] = xadj[i];  
	}
	xadj[xadj.size()-1] = xadj[xadj.size()-2] + numberOfElementsPerRow[numberOfVertices-1];


	#if DEBUG

	std::cout<<"adj :"<<std::endl;
	for(int i = 0; i<adj.size(); i++)
	{
		std::cout<<adj[i] << " ";
	}

	std::cout<<"xadj"<<std::endl;
	for(int i = 0; i<xadj.size(); i++)
	{
		std::cout<<xadj[i]<< " ";
	}
	std::cout<<std::endl;
	#endif
}

bool read_file(std::string &filename)
{
	edges = new std::vector<std::pair<int,int> >();
	printf("filename : %s \n",filename.c_str());
	std::ifstream inputFile(filename.c_str());
	if(inputFile.fail())
	{
		std::cout<<"File does not exists\n";
		return false;
	}
	std::string line;
	while(getline(inputFile, line))
	{
		std::istringstream lineStream (line);
		std::string word = "";
		int count = 0;
		int index1 = -1;
		int index2 = -2;
		while(lineStream >> word)
		{
			if(count == 0)
			{
				index1 = stoi(word);
			}
			else if(count == 1)
			{
				index2 = stoi(word);
			}
			count++;
			numberOfVertices = (index1 > numberOfVertices) ? index1 : numberOfVertices;
			numberOfVertices = (index2 > numberOfVertices) ? index2 : numberOfVertices;
		}
		std::pair<int,int> pair(index1,index2);
		std::pair<int,int> pair2(index2,index1);
		edges->push_back(pair);
		edges->push_back(pair2);
	}
	if(numberOfVertices != 0)
	{
		numberOfVertices++;
	}
	inputFile.close();
	return true;	
}

void clear_used_mem()
{
	delete edges;
}

int main(int argc, char* argv[])
{
	int nt = 60; //default
	int cycleLength = 0;
	numberOfVertices = 0;
	std::string filename = "";
	edges = NULL;
	if(argc > 1)
	{
		filename = std::string(argv[1]);
		cycleLength = stoi(std::string(argv[2]));
		nt = stoi(std::string(argv[3]));
	}
	if(read_file(filename))
	{
		std::cout<<"numberOfVertices = " << numberOfVertices << "\n";
		check_for_duplicates();
		std::vector<int> adj;
		std::vector<int> xadj (numberOfVertices+1,0);
		create_csr_representation(adj,xadj);		
		std::vector<int> values (adj.size(),1);
		std::vector<int> res_adj;
		std::vector<int> res_xadj(numberOfVertices+1);
		std::vector<int> res_val;
		omp_set_num_threads(nt);

		std::vector<int> trial_adj;
		std::vector<int> trial_xadj;
		std::vector<int> trial_values;
		if(nt != 0)
			find_result(adj,xadj,values,numberOfVertices,cycleLength);
		else
			find_result_gpu(adj, xadj, values, numberOfVertices, cycleLength);
	}
	return 0;
}
