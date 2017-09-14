#include<stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_OF_HASH 8
#define BLOOM_SIZE 12800
#define BLOOM_MEMORY_SIZE 400
#define W 16


struct sharedData{
    struct Bloom* b;
    int threadid;
};

struct hashData{
    int tid;
    int x;
    struct Bloom* bloom;
    int* arr;
};

struct Bloom{
    int size;
    int bit[BLOOM_MEMORY_SIZE];
    pthread_mutex_t mutexArr[BLOOM_MEMORY_SIZE];

};


void *bloomThread(void *threadid );
void *hashCalculate(void *x );
void sortHashes(int a[]);
void insertHash(int a[], struct Bloom* b);
int findHash( int a[], struct Bloom* b);
void getLocks(int a[], struct Bloom* b);
void modifyHash(int a[]);
void releaselocks(int a[], struct Bloom* b);
