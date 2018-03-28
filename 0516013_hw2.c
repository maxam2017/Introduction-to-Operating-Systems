#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>

struct timeval start, end;

int main(){
    printf("Input the matrix dimension: ");
    int n; //matrix size
    scanf("%d",&n);
    int *a,*b,*m;
    int id = shmget(IPC_PRIVATE,n*n*sizeof(int),IPC_CREAT|0660);
    int id_m = shmget(0,n*n*sizeof(int),IPC_CREAT | 0660);
    a = shmat(id,NULL,0);
    b = a;
    m = shmat(id_m,NULL,0);
    for(int i = 0;i<n*n;i++){
        *(a+i) = *(b+i) = i;
    }
    for(int i = 1;i<=16;i++){
        int p = n % i, q = n / i, r = 0;
        int tag = (p == 0)? 1:0;
        printf("Multiplying matrices using %2d process\n",i);
        gettimeofday(&start, 0);
        for(int j=0;j<i;j++){
            pid_t pid = fork();
            if(!tag && j+1 == i)
                q = p;
            if(pid == 0){
                for(int x = r;x<q;x++){
                    for(int y = 0;y<n;y++){
                        int tmp = 0;
                        for(int z = 0;z<n;z++){
                            tmp += a[x*n+z]*b[y+n*z];
                        }
                        m[x*n+y] = tmp;
                    }
                }
                
                exit(0);
            }
            r += q;
        }
        int k = i;
        while(k>0){
            wait(NULL);
            k--;
        }
        
        gettimeofday(&end, 0);
        unsigned sum = 0;
        for(int x = 0;x<n;x++){
            for(int y = 0;y<n;y++){
                sum += m[x*n+y];
            }
        }
        int sec = end.tv_sec - start.tv_sec;
        int usec = end.tv_usec - start.tv_usec;
        printf("Elapsed time: %12f ms , Checksum: %u\n",sec*1000+(usec/1000.0),sum);
    }

    shmdt(a);
    shmctl(id,IPC_RMID,NULL);
    shmdt(m);
    shmctl(id_m,IPC_RMID,NULL);
    return 0;
}
