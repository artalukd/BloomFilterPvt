#include "bloom2.h"

const int mul_a[8] = {2769, 4587, 8761, 9031, 6743, 7717, 9913, 8737};
const int add_b[8] = {767, 1657, 4057, 8111, 11149, 11027, 9901, 6379}; 

void *bloomThread(void *args)
{ 
   struct sharedData* sd = (struct sharedData *)args;
   int tid = sd->threadid;
   struct Bloom* bloom = sd->b;
   int x, opt;
   int i; 

   char filename[sizeof "file100.txt"];
   sprintf(filename, "file%03d.txt", tid); 
   FILE *fp = fopen(filename,"r");


   struct hashData* hdata = (struct hashData*)malloc(sizeof(struct hashData));
   hdata->bloom = sd->b;
   int arr[NUM_OF_HASH];
   hdata->arr =  arr;


   while (fscanf(fp, "%d %d\n",&opt, &x) == 2)
   {  pthread_t threads[NUM_OF_HASH];
    
        for( i = 0 ; i < NUM_OF_HASH ; i++ ) 
        {  
        hdata->tid = i;
        hdata->x = x;
        int rc = pthread_create(&threads[i], NULL, hashCalculate, (void *)hdata);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n in main", rc);
            exit(-1);
            }          
        } 

        for( i = 0 ; i < NUM_OF_HASH ; i++ ) {
        pthread_join(threads[i],NULL);
        }

        modifyHash(arr);
        sortHashes(arr);
        //printf("sort nd Hashed It %d %d %d %d %d %d %d %d\n", x, arr[0],arr[1],arr[2],arr[3],arr[4],arr[5],arr[6]);
        
        if(opt){
            insertHash(arr, bloom);
        }
        else
        {
        if(findHash(arr, bloom)){
            printf("%d may be present\n",x);
            FILE *fp = fopen("results2.txt","a");
            fprintf(fp, "may be present :%d\n",x);
            fclose(fp);
        }
        else
           continue;// printf("Not Found %d\n",x);
        }   
   }
   free(hdata);
   fclose(fp);
   free(sd);
   pthread_exit(NULL);
}

void sortHashes(int a[]){
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

void* hashCalculate(void* hdata){
    int m;
    struct hashData* hd = (struct hashData*)hdata;
    int id =  hd->tid;
    if(id<(NUM_OF_HASH/2))
        m = BLOOM_SIZE;
    else
        m = W;

    hd->arr[id] = (mul_a[id]*hd->x + add_b[id])%m;
    if(hd->arr[id] < 0)
        hd->arr[id]*=-1;
    //printf("calculating hash %d %d %d %d %d\n", hd->x,hd->arr[id],mul_a[id],add_b[id],id);
    pthread_exit(NULL);
}

void getLocks(int a[], struct Bloom* b){
    int i,j,d;
    for(i =1; i<NUM_OF_HASH;i++){

        d = a[i]/32;

        pthread_mutex_lock(&(b->mutexArr[d]));
    }
}

void releaseLocks(int a[], struct Bloom* b){
    int i,j;
    for(i =0; i<NUM_OF_HASH;i++){
        int d = a[i]/32;
        pthread_mutex_unlock(&(b->mutexArr[d]));
    }
}

void insertHash(int a[], struct Bloom* b){
    int i,m,d;
    getLocks(a,b);
    for(i =0; i<NUM_OF_HASH;i++){
        d = a[i]/32;
        m = a[i]%32;
        if((b->bit[d]&(1<<m)) > 0)
            continue;
        else
            b->bit[d] = (b->bit[d] | (1<<m));
    }
    releaseLocks(a,b);
}

int findHash(int a[], struct Bloom* b){
    int i,j =1, d,m;
    for(i =0; i<NUM_OF_HASH;i++){
        d = a[i]/32;
        m = a[i]%32;
        if((b->bit[d]&(1<<m)) > 0)
            continue;
        else
            j= 0;
    }
    return j;
}

void modifyHash(int a[]){
    int i,j =0;
        for(i =0; i<NUM_OF_HASH/2;i++){
            j+=NUM_OF_HASH/2 + i;
            a[j]+=a[i];
            int u = (int)(a[i]/W +0.999999999);
            if(a[j]>u)
                a[j] - W;
            }
}