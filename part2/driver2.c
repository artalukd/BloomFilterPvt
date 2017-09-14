#include "bloom2.h"
#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
//#include "bloom2.h"
#define NUM_OF_THREADS  3
#define SIZE_OF_BLOOM 30
#define UPPER_LIMIT_OF_NUM 996428
#define NUM_PER_THREAD  1000

int main(int argc, char *argv[])
{  
  time_t rndt;
   int j,i;
   srand((unsigned) time(&rndt));

   FILE *fp;
   for(j = 0; j<NUM_OF_THREADS; j++ )
   { //creating n files for n threads to read from
     char filename[sizeof "file100.txt"];
     sprintf(filename, "file%03d.txt", j);  
     fp = fopen(filename,"w");

   for( i = 0 ; i < NUM_PER_THREAD ; i++ ) 
   {
      fprintf(fp, "%d %d\n",rand() % 2, rand() % UPPER_LIMIT_OF_NUM);
   }
   fclose(fp);
   }

   struct Bloom* bfilter = (struct Bloom*)malloc(sizeof(struct Bloom)); 
   bfilter->size = 19997; 

   pthread_mutexattr_t Attr;
   pthread_mutexattr_init(&Attr);
   pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);

   for(i=0;i<BLOOM_MEMORY_SIZE;i++)
   {
      pthread_mutex_init(&(bfilter->mutexArr[i]), &Attr);
   }

   pthread_t threads[NUM_OF_THREADS];
   int rc;
   for(i=0;i<NUM_OF_THREADS;i++){
     struct sharedData* sdata = (struct sharedData*)malloc(sizeof(struct sharedData));
     sdata->b = bfilter;
     sdata->threadid = i;
     rc = pthread_create(&threads[i], NULL, bloomThread, (void *)sdata);
     if (rc){
       printf("ERROR; return code from pthread_create() is %d\n in main", rc);
       exit(-1);
       }
     }
   pthread_exit(NULL);
   return 0;
}