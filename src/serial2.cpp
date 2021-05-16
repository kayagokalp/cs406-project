
std::vector<std::vector<int> > serial2_paths;


void search_for_cycle(std::vector<int> &currentPath, std::vector<int> &adj, std::vector<int> &xadj, int req_length);

int get_number_of_cycles_opt(std::vector<int> &adj, std::vector<int> &xadj, int req_length)
{
	std::vector<int> currentPath;
	currentPath.push_back(0);
	search_for_cycle(currentPath, adj, xadj, req_length);
	for(int i = 0; i<serial2_paths.size(); i++)
	{
		std::vector<int> path = serial2_paths.at(i);
		for(int j = 0; j<path.size(); j++)
		{
			std::cout<<path.at(j)<< " ";
		}
		std::cout<<"\n";
	}
	return 0;
}


void search_for_cycle(std::vector<int> &currentPath, std::vector<int> &adj, std::vector<int> &xadj, int req_length)
{
	if(currentPath.size() == req_length+1)
	{
		if(currentPath.at(currentPath.size()-1) == currentPath.at(0))
		{
			serial2_paths.push_back(currentPath);
		}
		currentPath.pop_back();
		
	}else{
	std::cout<<" "<<xadj.size()<<std::endl;
	int index = currentPath[currentPath.size()-1];
	int start = adj[index];
	int end = adj[index+1];
	for(int i = start; i<end; i++)
	{
		std::cout<<" here "<<i<<std::endl;
		if(xadj[i] > currentPath.at(0) || (req_length == currentPath.size() && xadj[i] == currentPath[0]))
		{
			if(currentPath.size() == req_length && currentPath.at(0)!= xadj[i])
			{
				continue;
			}
			if(currentPath.size() > 2 && currentPath.at(index-1) == xadj[i])
			{
				continue;
			}
			std::cout<<"adding "<<xadj[i]<<" to";
                        for(int k =0; k<currentPath.size(); k++)
                        {
                        	std::cout<<currentPath[k] << " ";
                        }
                        std::cout<<std::endl;
                        currentPath.push_back(xadj[i]);
			search_for_cycle(currentPath, adj, xadj, req_length);
		}	
	}
	currentPath.pop_back();
	
	}
}
