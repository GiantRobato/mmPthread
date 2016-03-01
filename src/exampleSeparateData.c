#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define MATRIX_SIZE 16
#define NUM_THREADS 4

//Used for random number generation
#define RAN 10

//Function definitions
void fillMatrix(double ** m);

int main(void) {
	//seed random number generator
	srand(time(NULL));

	//create data
	double **a, **b, **c;
	double *aBlock, *bBlock, *cBlock; //create contiguous section of data

	//create pointers to data
	a = (double **)malloc(MATRIX_SIZE*sizeof(double *));
	b = (double **)malloc(MATRIX_SIZE*sizeof(double *));
	c = (double **)malloc(MATRIX_SIZE*sizeof(double *));

	//create points to blocks
	aBlock = (double *)malloc(MATRIX_SIZE*MATRIX_SIZE*sizeof(double));
	bBlock = (double *)malloc(MATRIX_SIZE*MATRIX_SIZE*sizeof(double));
	cBlock = (double *)malloc(MATRIX_SIZE*MATRIX_SIZE*sizeof(double));

	//intialize pointers for a, b, and c
	for(int i = 0; i < MATRIX_SIZE; i++){
		a[i] = aBlock+i*MATRIX_SIZE;
	}

	for(int i = 0; i < MATRIX_SIZE; i++){
		b[i] = bBlock+i*MATRIX_SIZE;
	}

	for(int i = 0; i < MATRIX_SIZE; i++){
		c[i] = cBlock+i*MATRIX_SIZE;
	}

	//Fill with random data
	fillMatrix(a);
	fillMatrix(b);
	fillMatrix(c);

	/****************************************************************
	* threading code
	****************************************************************/

	//create pthreads
	pthread_t *threads;
	threads = (pthread_t*)malloc(sizeof(pthread_t)*NUM_THREADS);

	//create containers for pthread input data
	//so we can do aThreadData[i] - pointer to matrix of size
	//SIZE/NUM_THREADS by SIZE
	double *** aThreadData = (double ***)malloc(NUM_THREADS*sizeof(double **));
	double *** bThreadData = (double ***)malloc(NUM_THREADS*sizeof(double **));
	double *** cThreadData = (double ***)malloc(NUM_THREADS*sizeof(double **));

	for(int i = 0; i < NUM_THREADS; i++){
		aThreadData[i] = (double **)malloc((MATRIX_SIZE/NUM_THREADS)*sizeof(double*));
	}

	//fill Data
	int k = 0;
	for(int i = 0; i < NUM_THREADS; i++){
		for(int j = 0; j < MATRIX_SIZE/NUM_THREADS; j++){
			aThreadData[i][j] = a[k++];
		}
	}

	//DEBUG

	printf("Data stored: \n");
	for(int i = 0; i < MATRIX_SIZE; i++){
		for(int j = 0; j < MATRIX_SIZE; j++){
			printf("%.2f ",a[i][j]);			
		}
		printf("\n");
	}

	printf("\nData stored in pthread: \n");
	for(int l = 0; l < NUM_THREADS; l++){
		printf("\nptr: %d\n",l);
		for(int i = 0; i < MATRIX_SIZE/NUM_THREADS; i++){
			for(int j = 0; j < MATRIX_SIZE; j++){
				printf("%.2f ",aThreadData[l][i][j]);
			}
			printf("\n");
		}
	}


	//create container for each slice of the data
	//double ** aMatSlice = (double **)malloc((MATRIX_SIZE/NUM_THREADS)*sizeof(double *));
	//double ** bMatSlice = (double **)malloc((MATRIX_SIZE/NUM_THREADS)*sizeof(double *));
	//double ** cMatSlice = (double **)malloc((MATRIX_SIZE/NUM_THREADS)*sizeof(double *));
//	double ** aMatSlice = (double **)malloc((MATRIX_SIZE/NUM_THREADS)*sizeof(double *));
//	double ** bMatSlice = (double **)malloc((MATRIX_SIZE/NUM_THREADS)*sizeof(double *));
//	double ** cMatSlice = (double **)malloc((MATRIX_SIZE/NUM_THREADS)*sizeof(double *));



	//for each thread
	for(int i = 0; i < NUM_THREADS; i++){
	
	}

	//cleanup
	free(a);
	free(b);
	free(c);
	free(aBlock);
	free(bBlock);
	free(cBlock);
	free(threads);

	for(int i = 0; i < NUM_THREADS; i++){
		free(aThreadData[i]);
	}



	return 0;
}

void fillMatrix(double **m){
	for(int i = 0; i < MATRIX_SIZE; i++){
		for(int j = 0; j < MATRIX_SIZE; j++){
			m[i][j] = (1 + rand() % RAN);
		}
	}
}
