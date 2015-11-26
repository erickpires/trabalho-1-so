#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define get_matrix_element(m,i,j) (m[i * matrix_size + j])
#define set_matrix_element(m,i,j,value) (m[i * matrix_size + j] = value)


void multiply_matrix(int);

int matrix_size = 3;

int* matrix_a;
int* matrix_b;
int* matrix_c;

void alloc_matrices() {

// Reference: http://stackoverflow.com/questions/13274786/how-to-share-memory-between-process-fork
//  glob_var = mmap(NULL, sizeof *glob_var, PROT_READ | PROT_WRITE,
//                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
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
	for(int i = 0; i < matrix_size; i++) {
		for(int j = 0; j < matrix_size; j++) {
			printf("%02d ", get_matrix_element(matrix, i, j));
		}
		printf("\n");
	}
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
	srand(time(NULL));

	alloc_matrices();


	init_matrix(matrix_a);
	init_matrix(matrix_b);

	puts("Matrix A");
	print_matrix(matrix_a);
	puts("Matrix B");
	print_matrix(matrix_b);

	for(int i = 0; i < matrix_size; i++) {
		pid_t pid = fork();

		if(pid == 0) {
			multiply_matrix(i);
			return 0; // All the child processes die here
		}
	}

	for(int i = 0; i < matrix_size; i++) {
		wait(NULL);
	}

	puts("Matrix C");
	print_matrix(matrix_c);

    return 0;
}