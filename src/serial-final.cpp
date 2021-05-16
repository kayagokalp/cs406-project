



void multiplyMatrixCSR(std::vector<int> &adj, std::vector<int> &xadj,int numberOfVertices, std::vector<std::vector<bool> > &mat, std::vector<std::vector<bool> > &resultingMatrix)
{

	for(int i = 0; i<numberOfVertices; i++)
	{
		std::vector<bool> rowVec(numberOfVertices,false);
		resultingMatrix.push_back(rowVec);
	}
	for(int i = 0; i<numberOfVertices; i++)
	{
		
		for(int j = adj[i]; j<adj[i+1]; j++)
		{
			for(int k = 0; k<numberOfVertices; k++)
			{
				resultingMatrix[i][k] = xadj[j] * mat[xadj[j]][k];
			}
		}
	}

}

void multiply_matrices_naive(std::vector<std::vector<int> > &mat1, std::vector<std::vector<int> > &mat2, int numberOfVertices, std::vector<std::vector<int> >&result)
{
	for(int i = 0; i<numberOfVertices; i++)
	{
		for(int j = 0; j<numberOfVertices; j++)
		{
			for(int k = 0; k<numberOfVertices; k++)
			{
				result[i][j] += mat1[i][j] * mat2[i][j];
			}
		}
	}
}
