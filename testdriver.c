//headers
#include "testbloom.h"
#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include <time.h>

//hyperparams
#define NUM_OF_THREADS  8
#define UPPER_LIMIT_OF_NUM 90000
#define NUM_PER_THREAD  3628800/NUM_OF_THREADS
#define BLOOM_SIZE 2559977
#define BLOOM_MEMORY_SIZE 40000
#define WORD 64
const int mul_a[8] = {2769, 4587, 8761, 9031, 6743, 7717, 9913, 8737};
const int add_b[8] = {767, 1657, 4057, 8111, 11149, 11027, 9901, 6379}; 

struct sharedData{
    struct Bloom* b;
    int tid;
};

struct hashData{
    int tid;
    int x;
    int* arr;
};

//main

// void* hashCalculate(void* hdata)
// {
//     int m;
//     struct hashData* hd = (struct hashData*)hdata;
//     int id =  hd->tid;
//     if(id<(NUM_OF_HASH/2))
//         m = BLOOM_SIZE;
//     else
//         m = WORD;

//     hd->arr[id] = (mul_a[id]*hd->x + add_b[id])%m;
//     if(hd->arr[id] < 0)
//         hd->arr[id]*=-1;
//     // if(hd->arr[id]>=m || hd->arr[id]< 0)
//     //     printf("matha kharap\n");
//     // //printf("calculating hash %d %d %d %d %d\n", hd->x,hd->arr[id],mul_a[id],add_b[id],id);
//     pthread_exit(NULL);
// }

void modifyHash(int a[])
{
    int i,j =0,u,k;
        for(i =0, j = NUM_OF_HASH/2 ; i<NUM_OF_HASH/2;i++, j++){
            a[j] = a[j]%WORD;
            a[j]+=a[i];
            u = (int)(a[i]/(WORD*1.0) + 0.999999999 );
            if(a[j]>(u*WORD))
                a[j] -= WORD;
            if(a[j]< 0)
                a[j]*=-1;
            // if(a[j]>=BLOOM_SIZE)
            //     printf("matha kharap     2.1\n");
            // if(a[j]< 0)
            //     printf("matha kharap     2.2\n");
            }
}

void sortHashes(int a[])
{
    int i,j, temp;
    for(i=0;i< NUM_OF_HASH/2;i++){
        for(j=0;j<NUM_OF_HASH/2;j++){
            if(a[i]<a[j])
            {   temp = a[i];
                a[i] =a[j];
                a[j] = temp;
                
                temp = a[i+NUM_OF_HASH/2];
                a[i+NUM_OF_HASH/2] =a[j+NUM_OF_HASH/2];
                a[j+NUM_OF_HASH/2] = temp;


            }
        }
    }
   
}

void *bloomThread(void *args)
{  clock_t begin = clock();
   struct sharedData* sd = (struct sharedData *)args;
   int tid = sd->tid;
   struct Bloom* bloom = sd->b;
   int x, opt, i; 

   char filename[sizeof "file100.txt"];
   sprintf(filename, "file%03d.txt", tid); 
   FILE *fp = fopen(filename,"r");


   struct hashData* hdata = (struct hashData*)malloc(sizeof(struct hashData));
   int arr[NUM_OF_HASH];
   hdata->arr =  arr;


   while (fscanf(fp, "%d %d\n",&opt, &x) == 2)
    {   // pthread_t threads[NUM_OF_HASH];
    
        for( i = 0 ; i < NUM_OF_HASH ; i++ ) 
        {  

        int m;
        //struct hashData* hd = (struct hashData*)hdata;
        //  int id =  hd->tid;
        if(i<(NUM_OF_HASH/2))
            m = BLOOM_SIZE;
        else
            m = WORD;

        arr[i] = (mul_a[i]*x + add_b[i])%m;
        if(arr[i] < 0)
            arr[i]*=-1;
        //hdata->tid = i;
        //hdata->x = x;
        // int rc = pthread_create(&threads[i], NULL, hashCalculate, (void *)hdata);
        // if (rc){
        //     printf("ERROR; return code from pthread_create() is %d\n in main", rc);
        //     exit(-1);
        //     }          
        // } 
        //hashCalculate((void *)hdata);
        }
        // for( i = 0 ; i < NUM_OF_HASH ; i++ ) {
        // pthread_join(threads[i],NULL);
        // }

        modifyHash(arr);
        sortHashes(arr);
        //printf("sort nd Hashed It %d %d %d %d %d %d %d %d\n", x, arr[0],arr[1],arr[2],arr[3],arr[4],arr[5],arr[6]);
        
        if(opt){
            insertHash(arr, bloom);
        }
        else
        {
        if(findHash(arr, bloom)){
            //printf("%d may be present\n",x);
        }
        else
           continue;
           //printf("Not Found %d\n",x);
        }   
   }

   clock_t end = clock();
   double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
   printf(" %lf  time spent in thread no : %d \n",time_spent, tid );
   free(hdata);
   fclose(fp);
   free(sd);
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{  
   time_t rndt;
   int j,i;
   srand((unsigned) time(&rndt));

   FILE *fp;
   for(j = 0; j<NUM_OF_THREADS; j++ )
   {
     //creating n files for n threads to read from
     char filename[sizeof "file100.txt"];
     sprintf(filename, "file%03d.txt", j);  
     fp = fopen(filename,"w");
     for( i = 0 ; i < NUM_PER_THREAD ; i++ ) 
         fprintf(fp, "%d %d\n",rand() % 2, rand() % UPPER_LIMIT_OF_NUM);
     fclose(fp);
   }

   struct Bloom* bfilter = createBloom(BLOOM_MEMORY_SIZE);

   pthread_t threads[NUM_OF_THREADS];
   int rc;

   for(i=0;i<NUM_OF_THREADS;i++){
     struct sharedData* sdata = (struct sharedData*)malloc(sizeof(struct sharedData));
     sdata->b = bfilter;
     sdata->tid = i;
     rc = pthread_create(&threads[i], NULL, bloomThread, (void *)sdata);
     if (rc){
       printf("ERROR; return code from pthread_create() is %d\n in main", rc);
       exit(-1);
       }
     }
   pthread_exit(NULL);

   

   return 0;



}


   
   
