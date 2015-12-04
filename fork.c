#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "timer.h"

#define get_matrix_element(m,i,j) (m[i * matrix_size + j])
#define set_matrix_element(m,i,j,value) (m[i * matrix_size + j] = value)


void multiply_matrix(int);

int matrix_size = 500;

int* matrix_a;
int* matrix_b;
int* matrix_c;

void alloc_matrices() {

// Reference: http://stackoverflow.com/questions/13274786/how-to-share-memory-between-process-fork
//
	matrix_a = (int*) mmap(NULL, matrix_size * matrix_size * sizeof(int), PROT_READ | PROT_WRITE,
	                       MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	matrix_b = (int*) mmap(NULL, matrix_size * matrix_size * sizeof(int), PROT_READ | PROT_WRITE,
	                       MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	matrix_c = (int*) mmap(NULL, matrix_size * matrix_size * sizeof(int), PROT_READ | PROT_WRITE,
	                       MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

void init_matrix(int* matrix) {
	for(int i = 0; i < matrix_size; i++) {
		for(int j = 0; j < matrix_size; j++) {
			int rand_value = rand() % 100;
			set_matrix_element(matrix, i, j, rand_value);
		}
	}
}

void print_matrix(int* matrix) {
	puts("[");
	for(int i = 0; i < matrix_size; i++) {
		for(int j = 0; j < matrix_size; j++) {
			printf("%02d ", get_matrix_element(matrix, i, j));
		}
		printf("\n");
	}
	puts("]");
	printf("\n");
}

inline void multiply_matrix(int line) {
	for(int j = 0; j < matrix_size; j++) {
		int accumulator = 0;
		for(int k = 0; k < matrix_size; k++) {
			accumulator += get_matrix_element(matrix_a, line, k) * get_matrix_element(matrix_b, k, j);
			// c[i][j] += a[i][k] * b[k][j];
		}
		set_matrix_element(matrix_c, line, j, accumulator);
	}
}


int main(int argc, char** argv){
	if(argc > 1) {
		matrix_size = atoi(argv[1]);
	}
	else {
		matrix_size = 3;
	}

	srand(time(NULL));

	alloc_matrices();

	init_matrix(matrix_a);
	init_matrix(matrix_b);

	double start;
	double finish;
	double elapsed;

	GET_TIME(start);

	for(int i = 0; i < matrix_size; i++) {
		pid_t pid = fork();

		if(pid == 0) {
			double proc_start;
			double proc_finish;

			GET_TIME(proc_start);
			multiply_matrix(i);
			GET_TIME(proc_finish);

			printf("Process_%02d,%lf,%lf\n", i, proc_start - start, proc_finish - start);
			// printf("Process: %d begin - %lf\n", i, proc_start - start);
			// printf("Process: %d finish - %lf\n", i, proc_finish - start);

			return 0; // All the child processes die here
		}
	}

	for(int i = 0; i < matrix_size; i++) {
		wait(NULL); // Waiting for all child processes.
	}

	GET_TIME(finish);
	elapsed = finish - start;

	// printf("%lf\n", elapsed);
	// printf("A = ");
	// print_matrix(matrix_a);
	// printf("B = ");
	// print_matrix(matrix_b);
	// printf("C = ");
	// print_matrix(matrix_c);

	// puts("A * B - C");

    return 0;
}
