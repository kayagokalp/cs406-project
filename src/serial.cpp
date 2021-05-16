#include <iostream>
#include <map>

int countCycles(std::vector<int> &adj, std::vector<int> &xadj, int n,int v);
std::vector<int> serial_paths;


int get_number_of_cycles_serial(std::vector<int> &adj, std::vector<int> &xadj,int length, int numberOfVertices, int vertexID)
{
	std::cout<<"size: "<<adj.size()<<std::endl;
	countCycles(adj,xadj,length,numberOfVertices);	
	int result = 0;
	for(int i = 0; i<serial_paths.size(); i++)
	{
		if(serial_paths.at(i) == vertexID)
		{
			result++;
		}
	}
	return result/2;
}


void DFS(std::vector<int>& adj, std::vector<int> &xadj, bool marked[], int n, 
               int vert, int start, int &count,int v,std::vector<int>& path) 
{ 
    // mark the vertex vert as visited 
    marked[vert] = true; 
  
    // if the path of length (n-1) is found 
    if (n == 0) { 
  
        // mark vert as un-visited to make 
        // it usable again. 
        marked[vert] = false; 
  
        // Check if vertex vert can end with 
        // vertex start 
	int start = adj[start];
	int end = adj[start+1];
	bool vertStart = false;
	for(int in = start; in<end; in++)
	{
		if(xadj[in] == vert)
		{
			std::cout<<"here";
			vertStart = true;
			break;
		}
	}
        if (vertStart) 
        {
        	count++;
		serial_paths.insert(std::end(serial_paths), std::begin(path), std::end(path));
        	return; 
        } else
        	return; 
    } 
  
    // For searching every possible path of 
    // length (n-1) 
    std::cout<<"here: "<< vert<<std::endl;
    for (int u = adj[vert]; u < adj[vert+1]; u++)
    {
	int i = xadj[u]; 
	std::cout<<i<<std::endl;
        if (!marked[i]) 
 	{
		// DFS for searching path by decreasing 
		// length by 1 
		std::vector<int> nextPath (path);
		nextPath.push_back(i);
		DFS(adj,xadj, marked, n-1, i, start, count,v,nextPath); 
	} 
    
    // marking vert as unvisited to make it 
    // usable again. 
    marked[vert] = false;
    } 
} 
  
// Counts cycles of length N in an undirected 
// and connected graph. 
int countCycles(std::vector<int>&adj, std::vector<int> &xadj, int n,int v) 
{ 
    // all vertex are marked un-visited intially. 
    bool marked[v]; 
    memset(marked, 0, sizeof(marked)); 
  
    // Searching for cycle by using v-n+1 vertices 
    int count = 0;
    std::cout<<"check 1"<<std::endl; 
    for (int i = 0; i < v - (n - 1); i++) {
    	std::cout<<"check "<<i<<std::endl; 
	std::vector<int> pth;
	pth.push_back(i);
        DFS(adj,xadj, marked, n-1, i, i, count,v,pth); 
  
        // ith vertex is marked as visited and 
        // will not be visited again. 
        marked[i] = true; 
    }
	#if DEBUG  
	int p_count = 0;
  	for(int i = 0; i<serial_paths.size(); i++)
	{
		if(i%n == 0)
		{	
			p_count++;
			printf("\n");
		}
		printf("%d ", serial_paths.at(i));
	}
	std::cout<<"SIZE : "<<count<<std::endl;
	#endif
	return count/2; 
}
