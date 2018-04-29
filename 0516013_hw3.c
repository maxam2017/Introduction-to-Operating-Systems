#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

struct timeval s1,s2,e1,e2;

int *A,*B; //array
int count =0;
sem_t work[15];
sem_t done,mutex; //semaphore

typedef struct arg{
  int n;
  int p;
  int r;
} ARG;

ARG* para;
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
  int p;
  int r;
} RNG;
RNG ran[15];

void push_range(int n,int p,int r){
  ran[n].p = p;
  ran[n].r = r;
}

void get_range(ARG* args){
  args->p = ran[args->n].p;
  args->r = ran[args->n].r;
}

/************* thread ***********/

void* thread_func(void* arg){
  ARG* args = (ARG*)arg;
  sem_wait(&work[args->n]);
  get_range(args);
  if(args->n < 7){
    int pivot = partition(args->p,args->r);
    int x = (pivot==args->p)? pivot:pivot-1;
    int y = (pivot==args->r)? pivot:pivot+1;
    push_range(args->n*2+1,args->p,x);
    push_range(args->n*2+2,y,args->r);
    sem_post(&work[args->n*2+1]);
    sem_post(&work[args->n*2+2]);
  }
  else{
    bubble_sort(args->p,args->r);
    sem_wait(&mutex);
    count++;
    if(count==8){
      sem_post(&done);
    }
    sem_post(&mutex);
  }
}
void* thread_one (void* arg){
    ARG* args = (ARG*)arg;
    for(int i=0;i<15;i++){
        get_range(args);
        if(args->n<7){
            int pivot = partition(args->p,args->r);
            int x = (pivot==args->p)? pivot:pivot-1;
            int y = (pivot==args->r)? pivot:pivot+1;
            push_range(args->n*2+1,args->p,x);
            push_range(args->n*2+2,y,args->r);
        }
        else{
            bubble_sort(args->p,args->r);
        }
        args->n++;
    }
    sem_post(&done);
}
int main(int argc, char **argv){
  //read file
  char filename[256];
  printf("filename:");
  scanf("%s",filename);
  FILE *in = fopen(filename,"r");
  if(!in){
    printf("file is not exist\n");
    return -1;
  }
  int n;
  fscanf(in,"%d",&n);
  A = malloc(n*sizeof(int));
  B = malloc(n*sizeof(int));
  for(int i=0;i<n;i++){
    fscanf(in,"%d",&A[i]);
    B[i]=A[i];
  }
  fclose(in);
  pthread_t t[15];
  para = malloc(15*sizeof(ARG));
  //initial semaphore
  for(int i=0;i<15;i++){
    sem_init(&work[i],0,0);
  }
  sem_init(&done,0,0);
  sem_init(&mutex,0,1);
  push_range(0,0,n-1);
  gettimeofday(&s1, 0); //mt sort begin
  for(int i=0;i<15;i++){
    para[i].n = i;
    pthread_create(&t[i],NULL,thread_func,(void*)&para[i]);
  }
  sem_post(&work[0]);
  sem_wait(&done);
  gettimeofday(&e1, 0); //mt sort done
  FILE *o1 = fopen("output1.txt","w");
  for(int i=0;i<n;i++){
    fprintf(o1,"%d ",A[i]);
  }
  fprintf(o1,"\n");
  fclose(o1);
  //restore array
  for(int i=0;i<n;i++){
      A[i]=B[i];
  }
  pthread_t k;
  gettimeofday(&s2, 0); //st sort begin
  pthread_create(&k,NULL,thread_one,(void*)&para[0]);
  sem_wait(&done);
  gettimeofday(&e2, 0); //st sort done
  int sec1 = e1.tv_sec - s1.tv_sec;
  int usec1 = e1.tv_usec - s1.tv_usec;
  int sec2 = e2.tv_sec - s2.tv_sec;
  int usec2 = e2.tv_usec - s2.tv_usec; 
  printf("MT sorting : %12f ms\n",sec1*1000+(usec1/1000.0));
  printf("ST sorting : %12f ms\n",sec2*1000+(usec2/1000.0));
  
  FILE *o2 = fopen("output2.txt","w");
  for(int i=0;i<n;i++){
    fprintf(o2,"%d ",A[i]);
  }
  fprintf(o2,"\n");
  fclose(o2);
  return 0;
}
