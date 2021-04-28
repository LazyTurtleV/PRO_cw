#include "mpi.h"
#include "serv_func.h"

#ifdef _DEBUG
	#define i1 19
	#define j1 15 	
	#define i2 15 
	#define j2 20
#else
	#define i1 240
	#define j1 56
	#define i2 56
	#define j2 333
#endif

#define PROC_NUM 8

#define A_MATRIX_ROW_TAG 1
#define B_MATRIX_COLUMN_TAG 2
#define C_MATRIX_EL_TAG	3
#define PROC_DATA_TRANSFER_TAG 3
#define BUFFER_SIZE 100

int main(int argc, char* argv[])
{
	int ProcNum, ProcRank, RecvRank;
	MPI_Datatype matrix_row, matrix_column;
	const char FILE_PATH[] = "C:\\Users\\Volodya\\Desktop\\";

	const int PROC_ORDER[PROC_NUM + 1] = {0, 1, 8, 7, 3, 2, 5, 6, 4}; //0 - I\O processor

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	if (ProcRank == 0)	
	{
		//is used for operations with string
		char buffer[BUFFER_SIZE];
		input_type type;
		
		printf("Please, enter input mode:\nM - manual; F - from file; R - with random numbers.\n");
		fflush(stdout);
		char mod = getchar();

		switch (mod) {
		case 'M':
			type = MANUAL;
			break;
		case 'F':
			type = FROM_FILE;
			break;
		case 'R':
			type = RANDOM;
			break;
		}

		if(type == MANUAL) printf("Matrix A: \n");
		int **A = input_matrix(i1, j1, type, concat_str(FILE_PATH, "A.dat", buffer, BUFFER_SIZE));
		if(type == MANUAL) printf("Matrix B: \n");
		int **B = input_matrix(i2, j2, type, concat_str(FILE_PATH, "B.dat", buffer, BUFFER_SIZE));
		int **C = aloc_con_matrix(i1, j2);

		printf("Calculating...\n");
		fflush(stdout);

		//A matrix row datatype
		MPI_Type_vector(j1, 1, 1, MPI_INT, &matrix_row);
		MPI_Type_commit(&matrix_row);
		//B matrix column datatype
		MPI_Type_vector(i2, 1, j2, MPI_INT, &matrix_column);
		MPI_Type_commit(&matrix_column);
		
		for (int i = 0; i < i1; i++) 
		{
			for (int j = 1; j < ProcNum; j++)
				MPI_Send(&A[i][0], 1, matrix_row, j, A_MATRIX_ROW_TAG, MPI_COMM_WORLD);

			for (int k = 0; k < j2; k++)
			{
				MPI_Send(&B[0][k], 1, matrix_column, 1, B_MATRIX_COLUMN_TAG, MPI_COMM_WORLD);

				//for (int g = 0; g < ProcNum; g++) 
				//{
					MPI_Recv(&C[i][k], 1, MPI_INT, MPI_ANY_SOURCE, C_MATRIX_EL_TAG, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
				//}
			}
		}

		//write C matrix
		FILE *res_out_file = NULL;
		errno_t err;
		if ((err = fopen_s(&res_out_file, concat_str(FILE_PATH, "C.dat", buffer, BUFFER_SIZE), "w")) != 0)
			printf("Err #%d.\nFailed to open file for writing C.\n", err);
		else 
		{
			print_matrix(C, i1, j2, res_out_file);
			fclose(res_out_file);
		}

		printf("Done!");
		fflush(stdout);

		free_matrix(A, i1, j1);
		free_matrix(B, i2, j2);
		free_matrix(C, i1, j2);
	}
	else 
	{
		int *A = calloc(j1, sizeof(int)), *B = calloc(j1, sizeof(int));

		for (int j = 0; j < i1; j++) 
		{
			MPI_Recv(A, j1, MPI_INT, 0, A_MATRIX_ROW_TAG, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

			for (int k = 0; k < j2; k++) 
			{
				int C = 0;

				MPI_Recv(B, j1, MPI_INT, find_priv_el(PROC_ORDER, ProcRank, PROC_NUM + 1), B_MATRIX_COLUMN_TAG, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

				for (int i = 0; i < j1; i++) C += A[i]*B[i];

				MPI_Send(&C, 1, MPI_INT, 0, C_MATRIX_EL_TAG, MPI_COMM_WORLD);
				
				//last process doesn't trunsfer its B column anywhere
				if (ProcRank != PROC_ORDER[PROC_NUM])
					MPI_Send(B, j1, MPI_INT, find_next_el(PROC_ORDER, ProcRank, PROC_NUM + 1), B_MATRIX_COLUMN_TAG, MPI_COMM_WORLD);
			}

		}

		free(A);
		free(B);
	}

	MPI_Finalize();
	return 0;
}
