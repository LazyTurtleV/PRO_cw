#include "serv_func.h"


__calc_type **input_matrix(int rows, int columns, input_type in_type, char *file_name)
{
	__calc_type **arr_ptr = aloc_con_matrix(rows, columns);
	FILE *output_file;

	switch (in_type)
	{
	case MANUAL:
		for(int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				printf("Enter element a%d%d\n", i, j);
				fflush(stdout);

				scanf_s("%I64d", &arr_ptr[i][j]);
			}
		}

		break;
	case RANDOM: {
		__calc_type _rand_start = 0, _rand_end = 0;
		printf("Please, enter rand_start and rand_end with space between:\n");
		fflush(stdout);
		scanf_s("%I64d %I64d", &_rand_start, &_rand_end);

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns; j++)
				arr_ptr[i][j] = _rand_start + rand() % (_rand_end - _rand_start);
		}

		break;
	}
	case FROM_FILE:;
		FILE *input_file = NULL;
		
		errno_t err;
		if ((err = fopen_s(&input_file, file_name, "r")) != 0)
		{
			printf("Err #%d\nFailed to open file\n", err);
		}

		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				fscanf_s(input_file, "%I64d", &arr_ptr[i][j]);
			}
		}

		fclose(input_file);
		break;
	}

	if (in_type != FROM_FILE)
	{
		errno_t err;
		if ((err = fopen_s(&output_file, file_name, "w")) != 0)
			printf("Err #%d\nFailed to open file\n", err);
		
		print_matrix(arr_ptr, rows, columns, output_file);

		fclose(output_file);
	}

	return arr_ptr;
}

//creates continuous in mem 2D dim matrix
__calc_type **aloc_con_matrix(int rows, int columns)
{
	__calc_type **arr_ptr = calloc(rows, sizeof(__calc_type*));
	arr_ptr[0] = calloc(rows * columns, sizeof(__calc_type));

	for (int i = 1; i < rows; i++) 
	{
		arr_ptr[i] = arr_ptr[i - 1] + columns;
	}

	return arr_ptr;
}




int find_next_el(int *arr_ptr, int el, int n)
{
	int el_id = -1;
	
	for (int i = 0; i < n; i++) 
	{
		if (arr_ptr[i] == el)
			el_id = i;
	}

	return arr_ptr[el_id + 1];
}

int find_priv_el(int *arr_ptr, int el, int n)
{
	int el_id = -1;

	for (int i = 0; i < n; i++)
	{
		if (arr_ptr[i] == el)
			el_id = i;
	}

	return arr_ptr[el_id - 1];
}


void free_matrix(__calc_type **arr_ptr, int rows, int columns)
{
	free(arr_ptr[0]);
	free(arr_ptr);
}

void print_matrix(__calc_type **arr_ptr, int rows, int columns, FILE *out_str)
{
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			char *delim = j == columns - 1 ? "%I64d" : "%I64d\t";
			fprintf(out_str, delim, arr_ptr[i][j]);
		}
		fputc('\n', out_str);
	}

	fflush(out_str);
}

void print_vector(__calc_type *arr_ptr, int lenght)
{
	for (int i = 0; i < lenght; i++)
		printf("%I64d\t", arr_ptr[i]);
	printf("\n");
	fflush(stdout);
}

char *concat_str(char* str1, char* str2, char* buffer, size_t buffer_size)
{
	snprintf(buffer, buffer_size, "%s%s", str1, str2);
	
	return buffer;
}