#ifndef SERV_FUNC_H
#define SERV_FUNC_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <Windows.h>
#include "mpi.h"

#define i1 240
#define j1 56
#define i2 56
#define j2 333

typedef enum {
	MANUAL,
	RANDOM,
	FROM_FILE
} input_type;

typedef __int64 __calc_type;
#define MPI_TRANSFER_TYPE MPI_LONG_LONG_INT

__calc_type **aloc_con_matrix(int rows, int columns);
__calc_type **input_matrix(int rows, int columns, input_type in_type, char* file_name);
void free_matrix(__calc_type **arr_ptr, int rows, int columns);

int find_next_el(int *arr_ptr, int el, int n);
int find_priv_el(int *arr_ptr, int el, int n);

void print_matrix(__calc_type **arr_ptr, int rows, int columns, FILE *out_str);
void print_vector(__calc_type *arr_ptr, int lenght);
char *concat_str(char* str1, char* str2, char* buffer, size_t buffer_size);

typedef struct Args_s
{
	int **A, **B;
} Args_s;

LPVOID WINAPI thread_routine(LPVOID args_s_ptr);

#endif