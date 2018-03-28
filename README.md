# Introduction-to-Operating-Systems
NCTU / 作業系統概論 / 張立平

### hw1
* prevent zombie process when excuting in background
* fork twice and kill the first one 
* the ppid of grandchild process is 1 (init) [adopted]

### hw2
* `ipcs -M` show shared memory size [shmmax]
* enlarge shared memory
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
