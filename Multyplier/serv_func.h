#ifndef SERV_FUNC_H
#define SERV_FUNC_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef enum {
	MANUAL,
	RANDOM,
	FROM_FILE
} input_type;

int	**aloc_con_matrix(int rows, int columns);
int **input_matrix(int rows, int columns, input_type in_type, char *file_name);
void free_matrix(int **arr_ptr, int rows, int columns);

int find_next_el(int* arr_ptr, int el, int n);
int find_priv_el(int* arr_ptr, int el, int n);

void print_matrix(int **arr_ptr, int rows, int columns, FILE *out_str);
void print_vector(int *arr_ptr, int lenght);
char *concat_str(char* str1, char* str2, char* buffer, size_t buffer_size);

#endif