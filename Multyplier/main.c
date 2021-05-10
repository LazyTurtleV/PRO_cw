#include "mpi.h"
#include "serv_func.h"

#define i1 240
#define j1 56
#define i2 56
#define j2 333

#define PROC_NUM 8
#define STR_BUFFER_SIZE 100

#define A_MATRIX_ROW_TAG 1
#define B_MATRIX_COLUMN_TAG 2
#define PROC_DATA_TRANSFER_TAG 3

int main(int argc, char* argv[])
{
	int ProcNum, ProcRank, RecvRank;
	MPI_Datatype matrix_row, matrix_column;
	const char FILE_PATH[] = "H:\\Studing\\University\\PRO\\RGR\\Multyplier\\Debug\\";

	const int PROC_ORDER[PROC_NUM + 1] = {0, 1, 8, 7, 3, 2, 5, 6, 4}; //0 - I\O processor

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	if (ProcRank == 0)	
	{
		//is used for operations with string
		char buffer[STR_BUFFER_SIZE];
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
		int **A = input_matrix(i1, j1, type, concat_str(FILE_PATH, "A.dat", buffer, STR_BUFFER_SIZE));
		if(type == MANUAL) printf("Matrix B: \n");
		int **B = input_matrix(i2, j2, type, concat_str(FILE_PATH, "B.dat", buffer, STR_BUFFER_SIZE));
		int **C = aloc_con_matrix(i1, j2);

		printf("Calculating...\n");
		fflush(stdout);

		//A matrix row datatype
		MPI_Type_vector(j1, 1, 1, MPI_INT, &matrix_row);
		MPI_Type_commit(&matrix_row);
		//B matrix column datatype
		MPI_Type_vector(i2, 1, j2, MPI_INT, &matrix_column);
		MPI_Type_commit(&matrix_column);
		
		for (int i = 0; i < i1; i += PROC_NUM) 
		{	
			//send A's rows to all processes
			int current_proc = find_next_el(PROC_ORDER, 0, PROC_NUM + 1);
			for (int j = 0; j < PROC_NUM; j++)
			{
				if (i + j < i1)
				{
					MPI_Rsend(&A[i + j][0], 1, matrix_row, current_proc, A_MATRIX_ROW_TAG, MPI_COMM_WORLD);
				}
				else
					//send some garbage to remaining processors
				{
					MPI_Rsend(&A[0][0], 1, matrix_row, current_proc, A_MATRIX_ROW_TAG, MPI_COMM_WORLD);
				}
				
				current_proc = find_next_el(PROC_ORDER, current_proc, PROC_NUM + 1);
			}

			//send procc#1 bi column
			for (int j = 0; j < j2; j++)
				MPI_Send(&B[0][j], 1, matrix_column, find_next_el(PROC_ORDER, 0, PROC_NUM + 1), B_MATRIX_COLUMN_TAG, MPI_COMM_WORLD);


			//calculate receives count: if A sent rows count a_s less then proc nums - than receives only a_s 
			const int it_count = ((i1 - i) < PROC_NUM)? (i1 - i) : PROC_NUM;
			//receive processors' responces
			current_proc = find_next_el(PROC_ORDER, 0, PROC_NUM + 1);
			for (int k = 0; k < it_count; k++)
			{	
				for (int j = 0; j < j2; j++)
				{	
					if (i + k < i1)
					{
						MPI_Recv(&C[i + k][j], 1, MPI_INT, current_proc, j, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
					}
				}
				current_proc = find_next_el(PROC_ORDER, current_proc, PROC_NUM + 1);
			}
		}


		//write C matrix to file
		FILE *res_out_file = NULL;
		errno_t err;
		if ((err = fopen_s(&res_out_file, concat_str(FILE_PATH, "C.dat", buffer, STR_BUFFER_SIZE), "w")) != 0)
			printf("Err #%d.\nFailed to open file for writing C.\n", err);
		else 
		{
			print_matrix(C, i1, j2, res_out_file);
			fclose(res_out_file);
		}

		printf("Done!\n");
		fflush(stdout);
	}
	else 
	{
		int *A = calloc(j1, sizeof(int)), *B = calloc(j1, sizeof(int));
		
		for (int j = 0; j < (int)ceil((double)i1 / PROC_NUM); j++) 
		{
			if (ProcRank == 7 && j == (int)ceil((double)i1 / PROC_NUM) - 1)
			{
				printf("Receiving A row...\n");
				fflush(stdout);
			}

			MPI_Recv(A, j1, MPI_INT, 0, A_MATRIX_ROW_TAG, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
			
			if (ProcRank == 7 && j == (int)ceil((double)i1 / PROC_NUM) - 1)
			{
				printf("Recieved A row!\n");
				fflush(stdout);
			}
			for (int k = 0; k < j2; k++) 
			{
				MPI_Recv(B, j1, MPI_INT, find_priv_el(PROC_ORDER, ProcRank, PROC_NUM + 1), B_MATRIX_COLUMN_TAG, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);

				int C = 0;
				for (int i = 0; i < j1; i++) 
					C += A[i]*B[i];

				MPI_Send(&C, 1, MPI_INT, 0, k, MPI_COMM_WORLD);
				
				//last process doesn't trunsfer its B column anywhere
				if (ProcRank != PROC_ORDER[PROC_NUM])
					MPI_Send(B, j1, MPI_INT, find_next_el(PROC_ORDER, ProcRank, PROC_NUM + 1), B_MATRIX_COLUMN_TAG, MPI_COMM_WORLD);
			}
		}
	}

	MPI_Finalize();
	return 0;
}
