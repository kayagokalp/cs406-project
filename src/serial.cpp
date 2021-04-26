#include <iostream>
#include <map>

int countCycles(bool** graph, int n,int v);
std::vector<int> paths;


int get_number_of_cycles_serial(bool** graph,int length, int numberOfVertices, int vertexID)
{
	//TODO(kaya) : find number of cycles with length length that contains vertexID.
	//TODO(kaya) : find all the cycles with length length. Count cycles that contain vertexID

	//TODO(kaya) : find number

	countCycles(graph,length,numberOfVertices);	
	int result = 0;
	for(int i = 0; i<paths.size(); i++)
	{
		if(paths.at(i) == vertexID)
		{
			result++;
		}
	}
	return result/2;
}


void DFS(bool **graph, bool marked[], int n, 
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
        if (graph[vert][start]) 
        {
        	count++;
		paths.insert(std::end(paths), std::begin(path), std::end(path));
        	return; 
        } else
        	return; 
    } 
  
    // For searching every possible path of 
    // length (n-1) 
    for (int i = 0; i < v; i++) 
        if (!marked[i] && graph[vert][i]) 
 	{
		// DFS for searching path by decreasing 
		// length by 1 
		std::vector<int> nextPath (path);
		nextPath.push_back(i);
		DFS(graph, marked, n-1, i, start, count,v,nextPath); 
	} 
  
    // marking vert as unvisited to make it 
    // usable again. 
    marked[vert] = false; 
} 
  
// Counts cycles of length N in an undirected 
// and connected graph. 
int countCycles(bool** graph, int n,int v) 
{ 
    // all vertex are marked un-visited intially. 
    bool marked[v]; 
    memset(marked, 0, sizeof(marked)); 
  
    // Searching for cycle by using v-n+1 vertices 
    int count = 0; 
    for (int i = 0; i < v - (n - 1); i++) {
	std::vector<int> pth;
	pth.push_back(i);
        DFS(graph, marked, n-1, i, i, count,v,pth); 
  
        // ith vertex is marked as visited and 
        // will not be visited again. 
        marked[i] = true; 
    }
	#if DEBUG  
  	for(int i = 0; i<paths.size(); i++)
	{
		if(i%n == 0)
		{
			printf("\n");
		}
		printf("%d ", paths.at(i));
	}
	#endif
	return count/2; 
}
