#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define get_matrix_element(m,i,j) (m[i * matrix_size + j])
#define set_matrix_element(m,i,j,value) (m[i * matrix_size + j] = value)


void multiply_matrix(int);

int matrix_size = 3;

int* matrix_a;
int* matrix_b;
int* matrix_c;

void alloc_matrices() {
	matrix_a = (int*) calloc(matrix_size * matrix_size, sizeof(int));
	matrix_b = (int*) calloc(matrix_size * matrix_size, sizeof(int));
	matrix_c = (int*) calloc(matrix_size * matrix_size, sizeof(int));
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

void* pthread_mul_matrix(void* _tid) {
	int tid = (int) _tid;

	multiply_matrix(tid);

	return NULL;
}


int main(int argc, char** argv){
	srand(time(NULL));

	alloc_matrices();


	init_matrix(matrix_a);
	init_matrix(matrix_b);

	puts("Matrix A");
	print_matrix(matrix_a);
	print_matrix(matrix_b);
	puts("Matrix B");

	pthread_t* threads = (pthread_t*) malloc(matrix_size * sizeof(pthread_t));

	for(int thread_id = 0; thread_id < matrix_size; thread_id++) {
		pthread_create(threads + thread_id, NULL, pthread_mul_matrix, (void*) thread_id);
	}

	for(int thread_id = 0; thread_id < matrix_size; thread_id++) {
		pthread_join(threads[thread_id], NULL);
	}

	puts("Matrix C");
	print_matrix(matrix_c);

    return 0;
}