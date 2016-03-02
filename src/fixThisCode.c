#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define MATRIX_SIZE 16384
#define NUM_THREADS 32
//Number of resolution loops
#define LOOPS		100

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
} sThreadData;

pthread_mutex_t lock;

int main(void) {
	clock_t t;

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
	* 5 Repeat steps 4 for number of resolution loops
	* 6 Stop Timer
	* 7 Display Data
	****************************************************************/

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

	/****************************************************************
	* 1 Convert data into slices
	*	-slice data
	*	-attach data into tData struct
	*	-DEBUG display before Data
	****************************************************************/

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

//	struct timeval stop,start;
//	gettimeofday(&start,NULL);
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
	if(pthread_mutex_init(&lock, NULL) != 0){
		printf("\nMUTEX init FAILED\n");
		return 1;
	}


	for(int loop = 0; loop < LOOPS; loop++){
		//lock shared resource
		

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

		while(numThreads > 0){
			//wait till numThreads gets to zero
		}
	}  //End Resolution Loops

	pthread_mutex_destroy(&lock);

	/****************************************************************
	* 6 Stop Timer
	****************************************************************/

	t = clock() - t;
	secPast = time(NULL) - secPast;
//	gettimeofday(&stop,NULL);
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

	double totalTime;
	totalTime = ((double)(t)/(double)CLOCKS_PER_SEC);

	double totalTime2;
	totalTime2 = ((double)(secPast));
	
//	long totalTime3;
//	totalTime3 = (stop.tv_sec - start.tv_sec) + ((stop.tv_usec - start.tv_usec)/1000000.0) + .5;
	
	printf("Total compute time using clock =%.3e seconds \n",totalTime);
	printf("Total compute time using time_t =%.3e seconds \n\n", totalTime2);
	printf("Total compute time using clock_gettime =%.3e seconds \n\n", elapsed);
	printf("Number of threads: %d\n", NUM_THREADS);
	printf("Matrix Size = %d-by-%d \n\n",MATRIX_SIZE,MATRIX_SIZE);

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
	pthread_mutex_lock(&lock);
//	printf("numThreads is: %d\n",++numThreads);
	pthread_mutex_unlock(&lock);

	//Matrix multiply
	int i;
	int j;
	for(i = 0; i < MATRIX_SIZE/NUM_THREADS; i++){
		for(j = 0; j < MATRIX_SIZE; j++){
			sTD->c[i][j] = sTD->a[i][j]*sTD->b[i][j];
		}
	}

	pthread_mutex_lock(&lock);
//	printf("numThreads is: %d\n",--numThreads);
	pthread_mutex_unlock(&lock);


	return NULL;
}
