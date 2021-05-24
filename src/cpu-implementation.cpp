#include <omp.h>

void multiply2(std::vector<int> &adj, std::vector<int> &xadj, std::vector<int> &tadj, std::vector<int> &txadj,std::vector<int> &values, std::vector<int> &tvalues, int size,std::vector<int> &result_adj, std::vector<int> &result_xadj, std::vector<int> &result_values);

void find_result(std::vector<int> &adj, std::vector<int> &xadj,std::vector<int> &values, int size, int path_length)
{
	std::vector<int> res_adj;
	std::vector<int> res_xadj(size+1, 0);
	std::vector<int> res_values;

	if(path_length == 3)
	{
		std::vector<int> pa_res_adj;
		std::vector<int> pa_res_xadj(size+1, 0);
		std::vector<int> pa_res_values;
		
		multiply2(adj,xadj,adj,xadj,values,values,size,pa_res_adj,pa_res_xadj,pa_res_values); // A*A
		multiply2(pa_res_adj, pa_res_xadj, adj, xadj, pa_res_values, values,size, res_adj, res_xadj, res_values);	//A^2 * A	

	}else if(path_length == 4)
	{
		std::vector<int> pa_res_adj;
		std::vector<int> pa_res_xadj(size+1, 0);
		std::vector<int> pa_res_values;
		
		multiply2(adj,xadj,adj,xadj,values,values,size,pa_res_adj,pa_res_xadj,pa_res_values); // A*A
		multiply2(pa_res_adj, pa_res_xadj, pa_res_adj, pa_res_xadj, pa_res_values, pa_res_values,size, res_adj, res_xadj, res_values);//A^2 * A^2	

	}else if(path_length == 5)
	{
		std::vector<int> pa_res_adj;
		std::vector<int> pa_res_xadj(size+1,0);
		std::vector<int> pa_res_values;

		std::vector<int> pa2_res_adj;
		std::vector<int> pa2_res_xadj(size+1,0);
		std::vector<int> pa2_res_values;

		multiply2(adj,xadj,adj,xadj,values,values,size,pa_res_adj,pa_res_xadj,pa_res_values); // A*A
		multiply2(pa_res_adj, pa_res_xadj, pa_res_adj, pa_res_xadj, pa_res_values, pa_res_values,size, pa2_res_adj, pa2_res_xadj, pa2_res_values);//A^2 * A^2	
		multiply2(pa2_res_adj, pa2_res_xadj, pa_res_adj, pa_res_xadj, pa2_res_values, pa_res_values,size, res_adj, res_xadj, res_values);//A^2 * A^2	
		
	}
	
	std::vector<int> result(size,0);
	for(int i = 0; i<res_xadj.size()-1; i++)
	{
		for(int index = res_xadj[i]; index<res_xadj[i+1]; index++)
		{
			if(i == res_adj[index])
			{
				result[i] = res_values[index]/(2*path_length);
			}
		}
	}
	
	for(int i = 0; i<result.size(); i++)
	{
		std::cout<<i<<"\t"<<result[i]<<std::endl;
	}
	

}

void multiply2(std::vector<int> &adj, std::vector<int> &xadj, std::vector<int> &tadj, std::vector<int> &txadj,std::vector<int> &values, std::vector<int> &tvalues, int size,std::vector<int> &result_adj, std::vector<int> &result_xadj, std::vector<int> &result_values)
{
	int number_of_threads = size;
	std::vector<std::vector<int> > res_adj(size);
	std::vector<std::vector<int> > res_values(size);
	std::cout<<"starting"<<std::endl;
	#pragma omp parallel for schedule(guided)	
	for(int i = 0; i< size; i++)
        {
		int pa_xadj_end = 0;
                std::vector<int> pa_adj;
                std::vector<int> pa_values;
                for(int col_iter = xadj[i]; col_iter<xadj[i+1]; col_iter++)
                {
                        int col_ptr = adj[col_iter];
			int place = 0;
                        for(int row_iter = txadj[col_ptr]; row_iter < txadj[col_ptr + 1]; row_iter++)
                        {
                                int row_ptr = tadj[row_iter];
                                int value = values[col_iter] * tvalues[row_iter];
                                if(pa_values.size() <= place)
                                {
                                        pa_values.push_back(value);
					pa_adj.push_back(row_ptr);
					pa_xadj_end++;
                                }else
                                {
                                        pa_values[place] += value;
                                }
				place++;
                        }
                }
		result_xadj[i+1] = pa_xadj_end;
                res_values[i] = (pa_values);
		res_adj[i] = pa_adj;
	}
	
/*	
	for(int i = 0; i< size; i++)
	{
		for(int j = 0; j<res_adj[i].size(); j++)
		{
			std::cout<<res_adj[i][j] << " "<<std::endl;
		}
		std::cout<<std::endl;
	}
*/
	for(int i = 0; i< size; i++)
	{
		for(int j = 0; j<res_values[i].size(); j++)
		{
			result_values.push_back(res_values[i][j]);
		}
	}
	for(int i = 0; i< size; i++)
	{
		for(int j = 0; j<res_adj[i].size(); j++)
		{
			result_adj.push_back(res_adj[i][j]);
		}
	}
/*	
	for(int i = 0; i<result_values.size(); i++)
	{
		std::cout<<result_values[i]<< " ";
	}
*/
	result_xadj[0] = 0;
	for(int i = 1; i<result_xadj.size(); i++)
	{
		result_xadj[i] += result_xadj[i-1];
	}
/*
	for(int i = 0; i<result_xadj.size(); i++)
	{
		std::cout<<result_xadj[i]<<" ";
	}
*/
}
