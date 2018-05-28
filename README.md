# Introduction-to-Operating-Systems
NCTU / 作業系統概論 / 張立平

### hw1
* prevent zombie process when excuting in background
* fork twice and kill the first one 
* the ppid of grandchild process is 1 (init) [adopted]

### hw2
* `ipcs -M` show shared memory size [shmmax] <byte>
* segment fault ==> enlarge shared memory
  * edit `/etc/sysctl.conf`
  * setting `shmmax` twice larger than before
    ```
    kern.sysv.shmmax=16777216
    kern.sysv.shmmin=1
    kern.sysv.shmmni=128
    kern.sysv.shmseg=32
    kern.sysv.shmall=4096
    ```
* `ipcs -m` show the info of processes which take up shared memory
* `ipcrm -m <PID>` clear the shared memory taken up by this process

### hw3
* compile
  * -pthead
  * -g : for gdb <multi-thread programming is difficult to debug>
* not fit requirement
  * using T<sub>0</sub> to fulfill pthread_join
  * this code using main function to recycle last 8 threads

### hw4
* using a semaphore[add] to restrict the dimension of thread pool
  * this semaphore post once when the dimension of thread pool increment
  * capacity = semaphore value
* using a semaphore[work] to wait sorting
  * post this semaphore when a task push into job list
* using a semaphore[done] to wait last 8 threads(bubble sort)
  * wait at main function (may not fulfill requirement)
  
### hw5
* page replacement
  > use <map> to implement hash [reference string to frame]
  * FIFO
  * LRU (least recently used)
    * using double linked list to record LRU memory
