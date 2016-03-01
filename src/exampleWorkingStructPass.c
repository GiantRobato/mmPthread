#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define MATRIX_SIZE 16
#define NUM_THREADS 8

//Used for random number generation
#define RAN 10

//Function definitions
void fillMatrix(double ** m);
void sliceMatrix(double*** threadData, double ** data);
void* threadFunction(void *chunk);
void* testFunction(void *data);

typedef struct sThreadData{
	double ** a;
	double ** b;
	double ** c;
} sThreadData;

typedef struct tmpData{
	double ** d;
} tmpData;

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

	/****************************************************************
	* threading code
	* Pseudo-Code
	*
	* 1 Convert data into slices
	* 2 Create threads
	* 3 Start Timer
	* 4 Start Matrix Multiply with slice data for each pthread
	* 5 Stop Timer
	* 6 Repeat steps 3-5 for number of resolution loops
	* 7 Display Time
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

	for(int i = 0; i < NUM_THREADS; i++){
		bThreadData[i] = (double **)malloc((MATRIX_SIZE/NUM_THREADS)*sizeof(double*));
	}

	for(int i = 0; i < NUM_THREADS; i++){
		cThreadData[i] = (double **)malloc((MATRIX_SIZE/NUM_THREADS)*sizeof(double*));
	}

	//reference previously made data

	//slices data matrix a into list of Matrices for pthreading
	sliceMatrix(aThreadData, a);
	sliceMatrix(bThreadData, b);
	sliceMatrix(cThreadData, c);

	//Create structs of data
	sThreadData *tData = (sThreadData*)malloc(NUM_THREADS*sizeof(sThreadData));

	for(int i = 0; i < NUM_THREADS; i++){
		tData[i].a = aThreadData[i];
		tData[i].b = bThreadData[i];
		tData[i].c = cThreadData[i];
	}

	//Do this for a certain number of resolution loops
	//DEBUG

	printf("Data stored: \n");
	for(int i = 0; i < MATRIX_SIZE; i++){
		for(int j = 0; j < MATRIX_SIZE; j++){
			printf("%.2f ",a[i][j]);			
		}
		printf("\n");
	}

	testFunction(&tData[0]);

	//run threads

	if(pthread_create(&threads[0],NULL,threadFunction, &tData[0])){
		printf("Error creating thread!\n");
		return 1;
	}


	//wait for pthread to finish
	if(pthread_join(threads[0],NULL)){
		printf("Error joining thread\n");
		return 2;
	}
	

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
	for(int i = 0; i < NUM_THREADS; i++){
		free(bThreadData[i]);
	}
	for(int i = 0; i < NUM_THREADS; i++){
		free(cThreadData[i]);
	}

	free(aThreadData);
	free(bThreadData);
	free(cThreadData);

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

	//Matrix multiply
	for(int i = 0; i < MATRIX_SIZE/NUM_THREADS; i++){
		for(int j = 0; j < MATRIX_SIZE; j++){
			sTD->c[i][j] = sTD->a[i][j]*sTD->b[i][j];
		}
	}

	return NULL;
}

void* testFunction(void *data){
	sThreadData* sTD = (sThreadData *) data;

	printf("Data stored: \n");
	for(int i = 0; i < MATRIX_SIZE/NUM_THREADS; i++){
		for(int j = 0; j < MATRIX_SIZE; j++){
//			printf("%.2f ",(*sTD).a[i][j]);
			printf("%.2f ",sTD->a[i][j]);
		}
		printf("\n");
	}
	
	return NULL;
}
