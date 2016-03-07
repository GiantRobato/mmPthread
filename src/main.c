/********************************************************************
*	License: MIT License (Dustin Mendoza)
*
*	Filename: main.c
*	Author:	Dustin Mendoza
*	Date: 3/7/16
*	Description: This program implements Matrix Multiply (Row times 
*	Columns) with pthreads
*
********************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define MATRIX_SIZE 64
#define NUM_THREADS 8
#define LOOPS		100 //Number of resolution loops

//Used for random number generation
#define RAN 10

//uncomment the life below to see debug options
//#define DEBUG

volatile int numThreads = 0;

//Function definitions
void fillMatrix(double ** m);
void sliceMatrix(double*** threadData, double ** data);
void* threadFunction(void *chunk);

typedef struct sThreadData{
	double ** a;
	double ** b;
	double ** c;
	int colStart;
} sThreadData;

pthread_mutex_t lock;

int main(void) {
	clock_t t;
	srand(time(NULL));	//seed random number generator

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

	/****************************************************************
	* threading code
	* Pseudo-Code
	*
	* 1 Convert data into slices
	* 2 Create threads
	* 3 Start Timer
	* 4 Start Matrix Multiply with slice data for each pthread
	* 5 Repeat steps 4 for number of resolution loops
	* 6 Stop Timer
	* 7 Display Data
	****************************************************************/

	/****************************************************************
	* 1 Convert data into slices
	*	-slice data
	*	-attach data into tData struct
	*	-DEBUG display before Data
	****************************************************************/

	//Create structs of data
	sThreadData *tData = (sThreadData*)malloc(NUM_THREADS*sizeof(sThreadData));

	for(int i = 0; i < NUM_THREADS; i++){
		tData[i].a = a;
		tData[i].b = b;
		tData[i].c = c;
		tData[i].colStart = i;
	}

	/****************************************************************
	* DEBUG - display before Data
	****************************************************************/
	#ifdef DEBUG
	printf("Data stored in A: \n");
	for(int i = 0; i < MATRIX_SIZE; i++){
		for(int j = 0; j < MATRIX_SIZE; j++){
			printf("%.2f ",a[i][j]);			
		}
		printf("\n");
	}

	printf("\nData stored in B: \n");
	for(int i = 0; i < MATRIX_SIZE; i++){
		for(int j = 0; j < MATRIX_SIZE; j++){
			printf("%.2f ",b[i][j]);			
		}
		printf("\n");
	}

	printf("\nData stored in C: \n");
	for(int i = 0; i < MATRIX_SIZE; i++){
		for(int j = 0; j < MATRIX_SIZE; j++){
			printf("%.2f ",c[i][j]);			
		}
		printf("\n");
	}
	#endif
	/****************************************************************
	* END DEBUG - display before Data
	****************************************************************/

	/****************************************************************
	* 2 Create threads
	*	-initialize pthread for number of threads
	****************************************************************/

	pthread_t *threads;
	threads = (pthread_t*)malloc(sizeof(pthread_t)*NUM_THREADS);

	/****************************************************************
	* 3 Start Timer
	****************************************************************/

	t = clock();
	time_t secPast = time(NULL);

	struct timespec start,finish;
	double elapsed;
	clock_gettime(CLOCK_MONOTONIC, &start);

	
	/****************************************************************
	* 4 Start Matrix Multiply with slice data for each pthread
	*	-run for number of resolution loops
	*	-start each thread
	*	-wait for each thread to finish
	****************************************************************/
	
	for(int loop = 0; loop < LOOPS; loop++){
		//Start threads
		for(int i = 0; i < NUM_THREADS; i++){
			if(pthread_create(&(threads[i]),NULL,threadFunction, &(tData[i]))){
				printf("Error creating thread!\n");
				return 1;
			}
		}

		//wait for all threads to finish
		for(int i = 0; i < NUM_THREADS; i++){
			if(pthread_join(threads[i],NULL)){
				printf("Error joining thread\n");
				return 2;
			}
		}

	}  //End Resolution Loops

	/****************************************************************
	* 6 Stop Timer
	****************************************************************/

	t = clock() - t;
	secPast = time(NULL) - secPast;
	clock_gettime(CLOCK_MONOTONIC, &finish);

	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec)/1000000000.0;

	/****************************************************************
	* 7 Display Data
	*	-print total time
	*	-print number of elements
	*	-print 
	****************************************************************/

	/****************************************************************
	* DEBUG - display after Data
	****************************************************************/
	#ifdef DEBUG

	printf("\nData after mm: \n");
	for(int i = 0; i < MATRIX_SIZE; i++){
		for(int j = 0; j < MATRIX_SIZE; j++){
			printf("%.2f ",c[i][j]);			
		}
		printf("\n");
	}
	#endif
	/****************************************************************
	* END DEBUG - display before Data
	****************************************************************/

	double avgTime = elapsed/((double)LOOPS);

	printf("Number of threads: %d\n", NUM_THREADS);
	printf("Matrix Size: %d-by-%d \n",MATRIX_SIZE,MATRIX_SIZE);
	printf("Number of Resolution Loops: %d\n\n",LOOPS);
	printf("Total compute time: %.3e seconds \n\n", elapsed);
	printf("Average Time per Matrix: %.3e\n",elapsed/(double)LOOPS);
	printf("Number of OPS: %.3e\n", ((double)(MATRIX_SIZE*MATRIX_SIZE*4))/avgTime);

	//cleanup
	free(a);
	free(b);
	free(c);
	free(aBlock);
	free(bBlock);
	free(cBlock);
	free(threads);

	free(tData);

	return 0;
}

void fillMatrix(double **m){
	for(int i = 0; i < MATRIX_SIZE; i++){
		for(int j = 0; j < MATRIX_SIZE; j++){
			m[i][j] = (1 + rand() % RAN);
		}
	}
}

void sliceMatrix(double*** threadData, double ** data){
	int k = 0;
	for(int i = 0; i < NUM_THREADS; i++){
		for(int j = 0; j < MATRIX_SIZE/NUM_THREADS; j++){
			threadData[i][j] = data[k++];
		}
	}
}


void* threadFunction(void *chunk){
	sThreadData *sTD = (sThreadData *) chunk;
	double sum = 0;
	for(int i = (sTD->colStart)*(MATRIX_SIZE/NUM_THREADS); i < (sTD->colStart+1)*(MATRIX_SIZE/NUM_THREADS); i++){
		for(int j = 0; j < MATRIX_SIZE; j++){
			for(int k = 0; k < MATRIX_SIZE; k++){
				sum += sTD->a[i][k]*sTD->b[k][j];
			}
			sTD->c[i][j] = sum;
			sum = 0;
		}
	}
	return NULL;
}
