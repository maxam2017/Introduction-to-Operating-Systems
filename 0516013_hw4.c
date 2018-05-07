#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

struct timeval s,e;

int *A,*B; //array
sem_t add,work,non_empty,mutex,mutex2,done; //semaphore
int d_tp = 0;
int count = 0;

typedef struct arg{
    int t; // level of sorting (0~3)
    int p; // sorting front
    int r; // sorting back
} ARG;

ARG para[8];
/************* sort ***************/

void swap(int* a,int* b){
  int tmp = *a;
  *a = *b;
  *b = tmp;
}
int partition(int p,int r){
  int i = p;
  for(int j=p+1;j<=r;j++){
    if(A[j]<=A[p]){
      i++;
      swap(&A[i],&A[j]);
    }
  }
  swap(&A[i],&A[p]);
  return i;
}

void bubble_sort(int p,int r){
  for(int i = r;i>p;i--)
    for(int j = p;j<i;j++){
      if(A[j]>A[j+1]){
        swap(&A[j],&A[j+1]);
      }
    }
}
/************* FIFO range ***********/

typedef struct range{
    int t;
    int p;
    int r;
} RNG;

RNG ran[15];

int i_rng = -1,g_rng = -1;

void push_range(int p,int r,int t){
    sem_wait(&mutex);
    i_rng++;
    i_rng %= 15;
    ran[i_rng].p = p;
    ran[i_rng].r = r;
    ran[i_rng].t = t;
    sem_post(&mutex);
    sem_post(&non_empty);
    sem_post(&work);
}

void get_range(ARG* args){
    sem_wait(&non_empty);
    sem_wait(&mutex);
    g_rng++;
    g_rng %= 15;
    args->p = ran[g_rng].p;
    args->r = ran[g_rng].r;
    args->t = ran[g_rng].t;
    sem_post(&mutex);
}

/************* thread ***********/
int k = 0;
void* thread_func(void* arg){
    ARG* args = (ARG*)arg;
    sem_wait(&add);
    while(1){
      sem_wait(&work);  // wait for all thread or waiting after the work is done
      get_range(args);
      if(args->t < 3){
          int pivot = partition(args->p,args->r);
          int x = (pivot==args->p)? pivot:pivot-1;
          int y = (pivot==args->r)? pivot:pivot+1;
          push_range(args->p,x,args->t+1);
          push_range(y,args->r,args->t+1);
      }
      else{
          bubble_sort(args->p,args->r);
          sem_wait(&mutex2);
            count++;
            if(count==8){
            sem_post(&done);
            }
          sem_post(&mutex2);
      }
    }
}

int main(int argc, char **argv){
    //read file
    FILE *in = fopen("input.txt","r");
    int n;
    fscanf(in,"%d",&n);
    A = malloc(n*sizeof(int));
    B = malloc(n*sizeof(int));
    for(int i=0;i<n;i++){
      fscanf(in,"%d",&A[i]);
      B[i]=A[i];
    }
    fclose(in);
    //thread
    pthread_t t[15];
    //initial semaphore
    sem_init(&work,0,0);
    sem_init(&non_empty,0,0);
    sem_init(&add,0,0);
    sem_init(&mutex,0,1);
    sem_init(&mutex2,0,1);
    sem_init(&done,0,0);
    //initial 8 threads
    for(int i=0;i<8;i++){
        pthread_create(&t[i],NULL,thread_func,(void*)&para[i]);
    }
    for(int i=0;i<8;i++){
      sem_post(&add); // add one dimesion of thread pool
      count = 0;
      gettimeofday(&s, 0);
      //push first job to trigger sorting
      push_range(0,n-1,0);
      sem_wait(&done);
      gettimeofday(&e, 0);
      int sec = e.tv_sec - s.tv_sec;
      int usec = e.tv_usec - s.tv_usec; 
      printf("Execution time of %d thread sorting : %12f ms\n",i+1,sec*1000+(usec/1000.0));
      //print result
      char fn[256]="output_*.txt";
      fn[7]=i+1+'0';
      FILE *o = fopen(fn,"w");
      for(int i=0;i<n;i++){
        fprintf(o,"%d ",A[i]);
      }
      //fprintf(o,"\n");
      fclose(o);
      //restore array
      for(int i=0;i<n;i++){
        A[i]=B[i];
      }
    }
    return 0;
}
