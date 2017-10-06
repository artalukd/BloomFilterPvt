#include "testbloom.h"

struct Bloom* createBloom(int size_memory){
    struct Bloom* bfilter = (struct Bloom*)malloc(sizeof(struct Bloom));
    bfilter-> bit = (uint64_t*)malloc(sizeof(uint64_t)*size_memory);
    bfilter->mutexArr = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*size_memory);

   pthread_mutexattr_t Attr;
   pthread_mutexattr_init(&Attr);
   pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);

   int i;
   for(i=0;i<size_memory;i++)
   {  bfilter-> bit[i] = 0;
      pthread_mutex_init(&(bfilter->mutexArr[i]), &Attr);
   }

    return bfilter;
}

void getLocks(int a[], struct Bloom* b){
    int i,j,d;
    for(i =1; i<NUM_OF_HASH;i++){
        d = a[i]/64;

        pthread_mutex_lock(&(b->mutexArr[d]));
    }
}

void releaseLocks(int a[], struct Bloom* b){
    int i,j,d;
    for(i =0; i<NUM_OF_HASH;i++){
        d = a[i]/64;
        pthread_mutex_unlock(&(b->mutexArr[d]));
    }
}

void insertHash(int a[], struct Bloom* b){
    uint64_t i,m,d,k = 1;
    getLocks(a,b);
    for(i =0; i<NUM_OF_HASH;i++){
        d = a[i]/64;
        m = a[i]%64;
         //printf("top  %" PRIu64 "   \n", b->bit[d]);
        k = (1<<m);
        if((b->bit[d] & k) > 0)
            continue;
        else
            b->bit[d] = (b->bit[d] | k);
            //printf("lololol %" PRIu64 "  %" PRIu64 "  %" PRIu64 "  %" PRIu64 "    \n", d,m,k,b->bit[d]);
    }
    releaseLocks(a,b);
}

int findHash(int a[], struct Bloom* b){
    uint64_t i,j =1, d,m, k =1;
    for(i =0; i<NUM_OF_HASH;i++){
        d = a[i]/64;
        m = a[i]%64;
        k = 1<<m;
        if((b->bit[d]&k) > 0)
            continue;
        else
            j= 0;
    }
    return j;
}
