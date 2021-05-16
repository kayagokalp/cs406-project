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
	//Matrix representation of the graph (duplicate free)

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
	int cycleLength = 0;
	numberOfVertices = 0;
	std::string filename = "";
	edges = NULL;
	if(argc > 1)
	{
		filename = std::string(argv[1]);
		cycleLength = stoi(std::string(argv[2]));
	}
	if(read_file(filename))
	{
		std::cout<<"numberOfVertices = " << numberOfVertices << "\n";
		check_for_duplicates();
		std::cout<<"duplicateDone\n";
		std::vector<int> adj;
		std::vector<int> xadj (numberOfVertices+1,0);
		create_csr_representation(adj,xadj);		
		std::vector<int> values (adj.size(),1);
		std::cout<<"CSR READY"<<std::endl;
		std::vector<int> res_adj;
		std::vector<int> res_xadj(numberOfVertices+1);
		std::vector<int> res_val;
		omp_set_num_threads(60);
//		multiply(adj,xadj,adj,xadj,values,values,numberOfVertices,res_adj,res_xadj,res_val);

		std::vector<int> trial_adj;
		std::vector<int> trial_xadj;
		std::vector<int> trial_values;
/*
		trial_adj.push_back(0);
		trial_adj.push_back(2);
		trial_adj.push_back(4);
		trial_adj.push_back(1);
		trial_adj.push_back(3);
		trial_adj.push_back(0);
		trial_adj.push_back(2);
		trial_adj.push_back(4);
		trial_adj.push_back(1);
		trial_adj.push_back(3);
		trial_adj.push_back(0);
		trial_adj.push_back(2);
		trial_adj.push_back(4);
		

		trial_xadj.push_back(0);
		trial_xadj.push_back(3);
		trial_xadj.push_back(5);
		trial_xadj.push_back(8);
		trial_xadj.push_back(10);
		trial_xadj.push_back(13);
		

		trial_values.push_back(2);
		trial_values.push_back(2);
		trial_values.push_back(2);

		trial_values.push_back(3);
		trial_values.push_back(3);

		trial_values.push_back(2);
		trial_values.push_back(2);
		trial_values.push_back(2);

		trial_values.push_back(3);
		trial_values.push_back(3);

		trial_values.push_back(2);
		trial_values.push_back(2);
		trial_values.push_back(2);
*/
		//multiply2(adj,xadj,adj,xadj,values,values,numberOfVertices,res_adj,res_xadj,res_val);
		find_result(adj,xadj,values,numberOfVertices,cycleLength);
/*
		std::cout<<"ADJ"<<std::endl;
		#if DEBUG

		for(int i = 0; i<res_adj.size(); i++)
		{
			std::cout<<res_adj[i]<<" ";
		}
		std::cout<<std::endl<<std::endl;
		
		std::cout<<"XADJ"<<std::endl;
		for(int i = 0; i<res_xadj.size(); i++)
		{
			std::cout<<res_xadj[i]<<" ";
		}
		std::cout<<std::endl<<std::endl;
		
		std::cout<<"VALUES"<<std::endl;
		for(int i = 0; i<res_val.size(); i++)
		{
			std::cout<<res_val[i]<<" ";
		}
		std::cout<<std::endl<<std::endl;		
		#endif
		std::vector<int> res2_adj;
		std::vector<int> res2_xadj(numberOfVertices+1);
		std::vector<int> res2_val;
		
		multiply2(res_adj,res_xadj,adj,xadj,res_val,values,numberOfVertices,res2_adj,res2_xadj,res2_val);
		std::cout<<"ADJ"<<std::endl;
		#if DEBUG

		for(int i = 0; i<res2_adj.size(); i++)
		{
			std::cout<<res2_adj[i]<<" ";
		}
		std::cout<<std::endl<<std::endl;
		
		std::cout<<"XADJ"<<std::endl;
		for(int i = 0; i<res2_xadj.size(); i++)
		{
			std::cout<<res2_xadj[i]<<" ";
		}
		std::cout<<std::endl<<std::endl;
		
		std::cout<<"VALUES"<<std::endl;
		for(int i = 0; i<res2_val.size(); i++)
		{
			std::cout<<res2_val[i]<<" ";
		}
		std::cout<<std::endl<<std::endl;		
		#endif	
*/			
			
	//	std::vector<std::vector<int> > mat1 = create_matrix_from_edges();
		//multiply_matrix(adj,xadj,values,adj,xadj,values,numberOfVertices);
		//find_res(adj,xadj,values,numberOfVertices,3);
//		std::cout<<"MATRIX READY"<<std::endl;

		//printf("\n");
		//printf("CSR READY\n");
		//std::vector<int> result(numberOfVertices, 0);
		//find_cycles(mat1,numberOfVertices,cycleLength,result);
			
//		std::vector<std::vector<int> > mat1 = create_matrix_from_edges();
		//multiply_matrix(adj,xadj,values,adj,xadj,values,numberOfVertices);
		//find_res(adj,xadj,values,numberOfVertices,3);
//		std::cout<<"MATRIX READY"<<std::endl;

		//printf("\n");
		//printf("CSR READY\n");
		//std::vector<int> result(numberOfVertices, 0);
		//find_cycles(mat1,numberOfVertices,cycleLength,result);
		//for(int k = 0; k<numberOfVertices; k++)
		//{
		//	std::cout << k <<" "<<result[k]<<std::endl;
		//}
		//printf("%d is result\n",get_number_of_cycles_serial(adjacencyMatrix,4, numberOfVertices,vertexIDtoSearch));
	}
		//clear_used_mem();
	return 0;
}
