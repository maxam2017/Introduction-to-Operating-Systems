#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //fork
#include <sys/types.h> //pid
#include <sys/wait.h> //wait
#include <fcntl.h> // open

typedef struct {
    int beWait;
    char* arg[5];// most 4 arguments
} Arg;

Arg parse(char* cmdline){
    char * p; 
    Arg a;
    p = strtok (cmdline," \n\t");
    int argc = 0;
    while (p != NULL){
        a.arg[argc++] = p;
        p = strtok (NULL, " \n\t");
    }
    a.beWait = 1;
    if(argc >0 && !strcmp(a.arg[argc-1],"&")){
        a.beWait = 0;
        argc--;
    }
    a.arg[argc++]=NULL;
    return a;
}

int main (){
    while(1){
        printf(">"); //prompt
        char cmdline[256]; //most 255 letters
        fgets (cmdline , 256 , stdin);
        char*pip = strchr(cmdline,'|');
        char*red  = strchr(cmdline,'>');
        if(pip){
            pid_t pid = fork();
            if(pid == 0){
                * pip = '\0';
                char* cmd1=cmdline,*cmd2 = pip+1;
                Arg a1  = parse(cmd1),a2 = parse(cmd2);
                int fd[2];
                pipe(fd);
                pid = fork();
                if(pid == 0){
                    close(fd[0]);
                    dup2(fd[1],1);
                    close(fd[1]);
                    execvp(a1.arg[0],a1.arg);
                }
                else{
                    close(fd[1]);
                    dup2(fd[0],0);
                    close(fd[0]);
                    execvp(a2.arg[0],a2.arg);
                }
            }
            else{
                wait(NULL);
            }
        }
        else if(red){
            pid_t pid = fork();
            * red = '\0';
            char* dest = strtok (red+1," \n\t");
            Arg a  = parse(cmdline);
            if(!a.beWait){
                if(pid == 0){
                    pid = fork();
                    if(pid == 0){
                        int k = open(dest, O_WRONLY | O_TRUNC | O_CREAT,0777);
                        close(0);
                        dup2(k,1);
                        close(k);
                        execvp(a.arg[0],a.arg);
                    }
                    else{
                        exit(0);
                    }
                }
                else{
                    wait(NULL);
                }
            }
            else{
                if(pid == 0){
                int k = open(dest, O_WRONLY | O_TRUNC | O_CREAT,0777);
                close(0);
                dup2(k,1);
                close(k);
                execvp(a.arg[0],a.arg);
                }
                else{
                    wait(NULL);
                }
            }
        }
        else{
            pid_t pid = fork();
            Arg a = parse(cmdline);
            if(!a.beWait){
                if(pid == 0){
                    pid = fork();
                    if(pid == 0){
                        execvp(a.arg[0],a.arg);
                    }
                    else{
                        exit(0);
                    }
                }
                else{
                    wait(NULL);
                }
            }
            else{
                if(pid == 0){
                    execvp(a.arg[0],a.arg);
                }
                else{
                    wait(NULL);
                }
            }
        }
    }
    return 0;
}
