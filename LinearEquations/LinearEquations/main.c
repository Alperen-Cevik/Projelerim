#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#pragma warning(disable : 4996)

typedef struct {
	char* variables;
	double** rows;
	size_t varCount;
	size_t rowCount;
} matrix_t;

matrix_t* create_matrix(char* variables, size_t varCount, size_t rowCount) {
	matrix_t* matrix = calloc(1, sizeof(matrix_t)); // allocating an instance of matrix struct
	matrix->variables = calloc(1, varCount);
	matrix->varCount = varCount; 
	matrix->rowCount = rowCount;

	matrix->rows = calloc(matrix->rowCount, sizeof(double*)); // allocating an array of pointers whose elements point to each row.
	matrix->rows[0] = calloc(matrix->varCount * matrix->rowCount, sizeof(double)); // allocating an array of doubles that stores the coefficients

	strcpy(matrix->variables, variables); // copying variable list into our struct
	
	for (size_t i = 1; i < matrix->rowCount; i++) // distributing rows to pointers
		matrix->rows[i] = matrix->rows[0] + i * matrix->varCount;

	return matrix;
}

void free_matrix(matrix_t* matrix) { // frees the matrix
	free(matrix->variables);
	free(matrix->rows[0]);
	free(matrix->rows);
	free(matrix);
}

int getline(FILE* stream, char** line, size_t chunkSize) { // reads a line from a file dynamically
	if (!stream || !line) // validating input
		return -1;

	int c;
	if ((c = fgetc(stream)) == EOF) // checking if EOF reached
		return -1;

	if (!chunkSize)
		chunkSize = 128; // defaulting chunk size

	size_t size = chunkSize;
	if (!*line) {
		*line = calloc(size, 1); // allocating our line string
		if (!*line) {
			fprintf(stderr, "Call to calloc failed\n"); // failure
			return -1;
		}
	}

	int count = 0;
	do {
		if (c == '\n')
			break;
		(*line)[count++] = c;
		if (count == size) {
			size += size / 2;
			char* new_line = realloc(*line, size); // reallocating line to store larger strings
			if (!new_line) {
				free(*line);
				fprintf(stderr, "Call to realloc failed\n"); // failure
				return -1;
			}
			*line = new_line;
			memset(*line + count, 0, chunkSize); // zeroing the rest of the string
		}
	} while ((c = fgetc(stream)) != EOF); // checking if EOF reached
	return count;
}

matrix_t* parse_coefficient_matrix_from_file(const char* filePath) { // parses equations into a matrix from a specified file
	FILE* file; // file pointer

	if (!(file = fopen(filePath, "r"))) {
		fprintf(stderr, "Cannot open file.\n"); // write error to stderr
		return NULL;
	}

	size_t lineCount = 0;

	char c;
	while (!feof(file)) // counting lines to pre allocate line array
		lineCount += (c = fgetc(file)) == '\n';

	lineCount++;

	rewind(file);

	char** lines = calloc(lineCount, sizeof(char*)); // allocating a pointer array whose members pointing to each line
	
	for (char** line = lines; getline(file, line++, 64) != -1;);

	fclose(file); // we re done with the file handle

	char* variables = NULL;
	size_t rowCount = 0;
	size_t maxVarCount = 0;

	/*
		Loop below extracts variables and counts rows.
	*/

	for (size_t i = 0; i < lineCount; i++) { // iterating over lines
		if (!lines[i] || !*lines[i]) // checking if line is empty
			continue;

		rowCount++;

		char* varList = calloc(strlen(lines[i]) + 1, 1); // allocating a temp var list to store variables
		size_t varCount = 0;
		for (char* ptr = lines[i]; *ptr; ptr++) { // iterating over equation
			if ((bool)isalpha(*ptr) && !strchr(varList, *ptr)) // isalpha evaluates if the char is in the a-z, A-Z range. strchr checks the existence of the var
				varList[varCount++] = *ptr; // storing variable
		}

		if (maxVarCount < varCount) {
			if (variables) // new max found, free the previous max list
				free(variables);
			maxVarCount = varCount;
			variables = varList;
		}
		else
			free(varList);
	}

	matrix_t* matrix = create_matrix(variables, maxVarCount + 1, rowCount); // creating our matrix
	free(variables);

	double** row = matrix->rows;
	for (size_t i = 0; i < lineCount; i++) { // iterating arrays
		if (!lines[i] || !*lines[i]) // checking if line is empty
			continue;

		double side_multiplier = 1.0; 
		double constant_term = 0.0; // constant term

		/*
			loop below parses the equation into coefficients and places them into the matrix
		*/

		char term[16] = { 0 };
		for (char* ptr = lines[i]; *ptr; ptr += strlen(term)) {
			double sign = 1.0;
			if (*ptr == '=') {
				side_multiplier = -1.0; // getting to the left side of the equation
				ptr++; // bypassing equality sign
			}
			if (*ptr == '-' || *ptr == '+') {
				if (*ptr == '-')
					sign = -1.0; // if our coef is negative
				ptr++; // bypassing signs
			}

			double coef = 1.0;
			char name = '\0';

			sscanf(ptr, "%16[^-+=]", term); // reading until a sign is found
			int itemsRead = sscanf(term, "%lf%c", &coef, &name); // reading a long float (double) and a char 

			if (!name) { // incase of no coefficient present or a constant term (x or 12 for e.g.)
				if (itemsRead) { // if the coef is read, then its a constant term
					constant_term += coef * sign * -side_multiplier;
					continue;
				}
				name = *term; // no constant term, a variable without a coef such as x (not 1x)
			}

			/*
				By evaluating this expression:
				"strchr(matrix->variables, name) - matrix->variables"
				we get the correct column index to put our coef into.
			*/
			(*row)[strchr(matrix->variables, name) - matrix->variables] += coef * sign * side_multiplier;
		}

		(*row)[matrix->varCount - 1] = constant_term; // storing constant term at the end of the row
		row++; // getting to the next row
	}

	for (size_t i = 0; i < lineCount; i++) {
		if (lines[i])
			free(lines[i]); // freeing line
	}
	free(lines); // freeing line array

	return matrix;
}

// -- row operations --

void swap_rows(double* dest, double* src, size_t len) { // swaps rows
	for (size_t i = 0; i < len; i++) {
		double tmp = dest[i];
		dest[i] = src[i];
		src[i] = tmp;
	}
}

void multilpy_row(double* dest, double multiplier, size_t len) { // multiplies a row by a number.
	for (size_t i = 0; i < len; i++)
		dest[i] *= multiplier;
}

void add_multiplied_row(double* dest, double* src, double multiplier, size_t len) { // adds a multiple of a row to another row.
	for (size_t i = 0; i < len; i++)
		dest[i] += src[i] * multiplier;
}

void reduce_matrix(matrix_t* matrix, size_t x, size_t y) { // converts matrix into its row reduced echelon form
	double* row = matrix->rows[y], val = row[x];
	if (val != 0.0) { // if value is non-zero:
		if (val != 1.0) // if value is not 1
			multilpy_row(row, 1 / val, matrix->varCount); // making it 1 by multiplying it with its inverse.
		for (size_t i = 0; i < matrix->rowCount; i++) { // iterating over rows
			if (i != y && matrix->rows[i][x] != 0.0)
				// making non-zero values in the same column zero:
				add_multiplied_row(matrix->rows[i], row, -matrix->rows[i][x], matrix->varCount);
		}
		if (x + 1 != matrix->varCount && y + 1 != matrix->rowCount) // checking if we are out of the matrix
			reduce_matrix(matrix, x + 1, y + 1); // starting over from 1 down 1 right
	}
	else { // if its zero
		size_t i; 
		for (i = y + 1; i < matrix->rowCount; i++) { // finding non zero elements below
			if (matrix->rows[i][x] != 0.0)
				break;
		}
		if (i != matrix->rowCount) { // if a non zero row is found, swap it with the current row
			swap_rows(matrix->rows[i], row, matrix->varCount);
			reduce_matrix(matrix, x, y); // start over from the same position
		}
		else if (x + 1 != matrix->varCount) // checking if we are out of the matrix
			reduce_matrix(matrix, x + 1, y); // starting over from 1 right
	}
}

bool check_solution(matrix_t* matrix, bool* infinite) {
	for (size_t i = 0; i < matrix->rowCount; i++) {
		bool pivotReached = 0;
		for (size_t j = 0; j < matrix->varCount - 1; j++) {
			if (matrix->rows[i][j] != 0 && pivotReached) { 
				// if a non zero coef is found after pivot, its a system w infinite solutions.
				*infinite = 1;
			}
			if (matrix->rows[i][j] == 1) // found pivot
				pivotReached = 1;
		}
		if (!pivotReached && matrix->rows[i][matrix->varCount - 1]) 
			// if there is no pivot and there is a constant term, it a system w no solutions.
			return 0;
	}
	return 1;
}

/*
Incase you wanna test:

Single solution:
3x=7+y
y=8-2x

x+y+z=2
5x+2y+2z=13
6x-4y+5z=31

No solutions:
3x+y+3z=2
x+2z=-3
2x+y+z=4

Infinite:
a+2b+d=3
-a-2b+c-2f=-2
-2a-4b+c-d-2f=-5
*/

int main(void) {
	matrix_t* matrix = parse_coefficient_matrix_from_file("equations.txt");

	assert(matrix != NULL); // terminating if parsing failed

	reduce_matrix(matrix, 0, 0);

	bool infinite = 0;
	if (check_solution(matrix, &infinite)) {
		if (!infinite) {
			puts("Solution:");
			for (size_t i = 0; i < matrix->rowCount; i++) // printing solutions
				printf("%c: %.3lf\n", matrix->variables[i], matrix->rows[i][matrix->varCount - 1]);
		}
		else
			puts("This system has infinite solutions.");
	}
	else
		puts("This system has no solutions.");

	free_matrix(matrix);
}