#include<stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <inttypes.h>
#define NUM_OF_HASH 6

struct Bloom{
    uint64_t *bit;
    pthread_mutex_t *mutexArr;
};



struct Bloom* createBloom(int size_memory);
void insertHash(int a[], struct Bloom* b);
int findHash( int a[], struct Bloom* b);
void getLocks(int a[], struct Bloom* b);
void releaselocks(int a[], struct Bloom* b);