#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

/* Initializing all variables */

pthread_mutex_t mutex;
sem_t empty, full;
int counter = 0;
int in = 0;
int out = 0;
int producerArray[500], consumerArray[500];
int N, P, C, X, Ptime, Ctime, Y, Z;
int *buffer;
void *producer(void *param);
void *consumer(void *param);
int item, citem;
int overConsume;
int pCounter = 0;
int cCounter = 0;

/* 
 * Function to remove item.
 * Item removed is returned
 */
int dequeue_item(){
	
	if(counter != 0){
       		citem = buffer[out];
		//printf("\n\n citem is : %d\n\n", citem);
		out = (out + 1) % N;
		counter--;
		return citem;
	}
	else{
		return -1;
	}

}

/* 
 * Function to add item.
 * Item added is returned.
 * It is up to you to determine
 * how to use the ruturn value.
 * If you decide to not use it, then ignore
 * the return value, do not change the
 * return type to void. 
 */
int enqueue_item(int item){
	
	if(counter < N){
		buffer[in] = item;
		in = (in + 1) % N;
		counter++;
		return 0;
	}
	else{	
		return -1;
	}
}

void *producer(void *param){
	int pid = (int)(intptr_t)param;
	
	int i = 0;
	
	while(i < X){
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);
		item += 1;
		int result = enqueue_item(item);
		if(result == -1){
			fprintf(stderr, "Producer error\n");
		}
		else{
			printf("%d was created by producer-> %d\n", item, (pid + 1));
		}
	
		producerArray[pCounter] = item;
		pCounter++;
		i++;
		sleep(Ptime);
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
	}
}



void *consumer(void *param){
	int cid = (int)(intptr_t)param;
	//printf("\n\n Why is cid : %d\n\n", cid);
	int i = 0;
		
	/* loop for each individual consumer need to check if consumer should over consume */
	if(cid < Z){
		while(i < Y + 1){
			sem_wait(&full);
			pthread_mutex_lock(&mutex);
			int result = dequeue_item();
		
			if(result == -1){
				fprintf(stderr, "Consumer error\n");
			}

			else{
				printf("%d was consumed by consumer-> %d\n", result, (cid + 1));
			}

			consumerArray[cCounter] = result;
			//printf("\n result stored is : %d\n", result);
			cCounter++;
			i++;
			sleep(Ctime);	
			pthread_mutex_unlock(&mutex);
			sem_post(&empty);
		}
	}

	else{
		while(i < Y){
		sem_wait(&full);
		pthread_mutex_lock(&mutex);
		int result = dequeue_item();
		
		if(result == -1){
			fprintf(stderr, "Consumer error\n");
		}

		else{
			printf("%d was consumed by consumer-> %d\n", result, (cid + 1));
		}

		consumerArray[cCounter] = result;
		//printf("\n result stored is : %d\n", result);
		cCounter++;
		i++;
		sleep(Ctime);	
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);
	
	}
	//printf("\n\nbottom loop exited\n");	
	}
	
}


	
int main(int argc, char** argv) {
	/* Program must accept 6 args */
	if(argc != 7){
		fprintf(stderr, "Incorrect number of arguments passed");
	}
			
	N = atoi(argv[1]);
	P = atoi(argv[2]);
	C = atoi(argv[3]);
	X = atoi(argv[4]);
	Ptime = atoi(argv[5]);
	Ctime = atoi(argv[6]);
	Y = (P*X)/C;
	Z = (P*X) % C;
	
	if(Z == 0){
		overConsume = Z;
	}

	else{
		overConsume = 1;
	}

	/*init*/
	pthread_mutex_init(&mutex, NULL);
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, N);
	pthread_t Ptid[P];
	pthread_t Ctid[C];
	buffer = (int *)malloc(sizeof(int) * N);

	/* I like number better than date and time */
	time_t start_t;
        time_t end_t;
	struct tm *ts;
	char buf[80];	
	start_t = time(NULL);
	ts = localtime(&start_t);
	//start_t = (intptr_t)ts;
	printf("Current Time is: %s\n\n", asctime(ts));
	printf("Number of Buffers :   %d\n", N);
	printf("Number of Producers :   %d\n", P);
	printf("Number of Consumers :   %d\n", C);
	printf("Number of items Produced by each producer :   %d\n", X);
	printf("Number of items consumed by each consumer :   %d\n", Y);
	printf("Over consume on? :   %d\n", overConsume);
	printf("Over consume amount :   %d\n", Z);
	printf("Time each Producer Sleeps (seconds) :   %d\n", Ptime);
	printf("Time each Consumer Sleeps (seconds) :   %d\n\n", Ctime);
	
	for(int i = 0; i < P; i++){

		//printf("\n %d <--- \n", &Ptid[i]);
		//printf("\n\n i is %d\n\n", i);
		pthread_create(&Ptid[i], NULL, producer, (void *)(intptr_t) i);
	}
	for(int i = 0; i < C; i++){
		//printf("\n\n i for consumer is %d\n\n", i);
		pthread_create(&Ctid[i], NULL, consumer, (void *)(intptr_t) i);
	}
	for(int i = 1; i <= P; i++){
		pthread_join(Ptid[i - 1], NULL);
		printf("Producer Thread joined: %d\n", i);
	}
	for(int i = 1; i <= C; i++){
		pthread_join(Ctid[i - 1], NULL);
		printf("Consumer Thread joined: %d\n", i);
	}
	pthread_mutex_destroy(&mutex);
	sem_destroy(&full);
	sem_destroy(&empty);
	
	end_t = time(NULL);
	ts = localtime(&end_t);
	//end_t = (intptr_t)ts;
	/*print time*/
	printf("\nCurrent time is: %s\n", asctime(ts));

	printf("Producer Array  |  Consumer Array\n");	
	for(int i = 0; i < (P*X); i++){
		printf("%d     |     %d\n", producerArray[i], consumerArray[i]);
	}
	printf("\n");

	int check = 0;
	for(int i = 0; i < (P*X); i++){
		int p = producerArray[i];
		int c = consumerArray[i];
		if(p != c){
			check = -1;
		}
	}
	if(check == -1){
		fprintf(stderr, "Arrays don't match still\n");
	}
	else{
		printf("Consume and Produce Arrays match\n");
	}
	long total_t = (end_t - start_t);
	printf("Total Runtime: %ld seconds\n", total_t);
	exit(0);
}
