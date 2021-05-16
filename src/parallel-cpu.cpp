//#include <omp.h>
//#include <queue>

void count_length_3_cycles_from(int vertexIDtoStartSearch, std::vector<int> &adj, std::vector<int> &xadj);
void count_length_4_cycles_from(int vertexIDtoStartSearch, std::vector<int> &adj, std::vector<int> &xadj);

int count_number_of_parallel(int vertexIDtoStartSearch, std::vector<int> &adj, std::vector<int> &xadj, int requiredLength,int numberOfVertex)
{

	if(requiredLength == 3)
	{
		count_length_3_cycles_from(vertexIDtoStartSearch, adj,xadj);	
	}else if(requiredLength == 4)
	{
		for(int i = 0; i<numberOfVertex; i++)
			count_length_4_cycles_from(i, adj, xadj);
	}
	return 0;
}


void count_length_3_cycles_from(int vertexIDtoStartSearch, std::vector<int> &adj, std::vector<int> &xadj)
{
//	std::vector<std::vector<int> > paths;
	for(int i = adj[vertexIDtoStartSearch]; i<adj[vertexIDtoStartSearch+1]; i++)
	{
		int b = xadj[i];
		std::cout<<vertexIDtoStartSearch<< " "<<b<<std::endl;
		if(b > vertexIDtoStartSearch)
		{
			for(int j = adj[b]; j<adj[b+1]; j++)
			{
				int c = xadj[j];
				std::cout<<vertexIDtoStartSearch<< " "<<b<< " "<< c<<std::endl;
				if(c > b)
				{
					for(int k = adj[c]; k<adj[c+1]; k++)
					{
						int last = xadj[k];
						if(last == vertexIDtoStartSearch)
						{
							std::cout<<vertexIDtoStartSearch << " "<< b<< " "<< c<<std::endl;
						}
					}
				}
			}
		}
	}
}


void count_length_4_cycles_from(int vertexIDtoStartSearch, std::vector<int> &adj, std::vector<int> &xadj)
{
//      std::vector<std::vector<int> > paths;
        for(int i = adj[vertexIDtoStartSearch]; i<adj[vertexIDtoStartSearch+1]; i++)
        {
                int b = xadj[i];
                if(b > vertexIDtoStartSearch)
                {
                        for(int j = adj[b]; j<adj[b+1]; j++)
                        {
                                int c = xadj[j];
                        	if(c != vertexIDtoStartSearch)        
				{
                                        for(int k = adj[c]; k<adj[c+1]; k++)
                                        {
                                                int d = xadj[k];
                                                if(d != b)
						{
							for(int l = adj[d]; l<adj[d+1]; l++)
							{
								int last = xadj[l];
								if(last == vertexIDtoStartSearch)
								{
									std::cout<<vertexIDtoStartSearch<<" "<< b<< " "<< c<< " "<<d<<std::endl;
									break;
								}
							}
                                                }
                                        }
                                }
                        }
                }
        }

}


