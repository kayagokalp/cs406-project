#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#define DEBUG 1 


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

	//TODO(kaya): a-a edges should be deleted.
 
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

void create_matrix_from_edges()
{
	//Matrix representation of the graph (duplicate free)
	//matrixOfEdges.push_back();
}

void create_csr_representation()
{
	//TODO(kaya) : create csr
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
	numberOfVertices = 0;
	std::string filename = "";
	edges = NULL;
	if(argc > 1)
	{
		filename = std::string(argv[1]);
	}
	if(read_file(filename))
	{
		std::cout<<" numberOfVertices = " << numberOfVertices << "\n";
		check_for_duplicates();
	}
	clear_used_mem();
	return 0;
} 
