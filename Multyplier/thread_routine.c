#include "serv_func.h"

LPVOID WINAPI thread_routine(LPVOID args_s_ptr)
{
	Args_s args = *(Args_s*)args_s_ptr;
	__calc_type **C = aloc_con_matrix(i1, j2);

	clock_t timer;
	timer = clock();

	for (int i = 0; i < i1; i++)
	{
		for (int j = 0; j < i2; j++)
		{
			for (int k = 0; k < j1; k++)
			{
				C[i][j] += args.A[i][k] + args.B[k][j];
			}
		}
	}
	
	timer = clock() - timer;
	printf("It took to perform sequential algoritm %e s\n", ((double)timer) / CLOCKS_PER_SEC);
	fflush(stdout);
}