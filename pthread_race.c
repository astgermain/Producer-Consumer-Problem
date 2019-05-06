#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
/**
 * THESE DEFINE VALUES CANNOT BE CHANGED.
 * DOING SO WILL CAUSE POINTS TO BE DEDUCTED
 * FROM YOUR GRADE
 */
 /** BEGIN VALUES THAT CANNOT BE CHANGED */
#define MAX_THREADS 36
#define MAX_ITERATIONS 22
/** END VALUES THAT CANNOT BE CHANGED */


/**
 * use this struct as a parameter for the
 * nanosleep function.
 * For exmaple : nanosleep(&ts, NULL);
 */
struct timespec ts = {0, 150000};
static int sharedVar = 0;

void *adder(void *arg) {
	int temp;
	int tid = *((int*)arg);
	for (int i=0; i < MAX_ITERATIONS; i++){
		
		temp = sharedVar;
		nanosleep(&ts, NULL);
		temp = temp + 10;
		sharedVar = temp;
		printf("Current Value written to Global Variables by ADDER thread id: %d is %d\n", tid, temp);
	}
}

void *subtractor(void *arg) {
	int temp;
	int tid = *((int*)arg);
	for (int i=0; i < MAX_ITERATIONS; i++){
		temp = sharedVar;
		temp = temp - 10;
		sharedVar = temp;
		printf("Current Value written to Global Variables by SUBTRACTOR thread id: %d is %d\n", tid, temp);
	}
}

int
main(int argc, char** argv)
{	
	pthread_t tid[MAX_THREADS];
	for (int i = 0; i < MAX_THREADS; i++){
		if (i%2 == 0){
			pthread_create(&tid[i], NULL, adder, (void *)&i);
		}
		else{
			pthread_create(&tid[i], NULL, subtractor, (void *)&i);
		}
	}
	for(int j = 0; j < MAX_THREADS; j++){
		pthread_join(tid[j], NULL);
	}
	printf("Final Value of Shared Variable : %d\n", sharedVar);
    return 0;
}
