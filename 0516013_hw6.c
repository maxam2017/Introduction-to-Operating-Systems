#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h> 
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
int w,x,y,z;
unsigned long long inode;
double size_max,size_min;
char *filename,*pathname;
void parse_arg(int argc, char **argv){
    if(argc>1)
        pathname = argv[1];
    const char *opt = "i:n:s:t:";
    struct option opts[] = {
        {"inode", 1, NULL, 'i'},
        {"name", 1, NULL, 'n'},
        {"size_min", 1, NULL, 's'},
        {"size_max", 1, NULL, 't'}
    };
    int v;
    while((v = getopt_long_only(argc, argv, opt, opts, NULL)) != -1){
        switch(v){
            case 'i':
                w = 1;
                inode = (unsigned long long)atoll(optarg);
                break;
            case 'n':
                x = 1;
                filename = optarg;
                break;
            case 's':
                y = 1;
                size_min = atof(optarg)*1024;
                break;
            case 't':
                z = 1;
                size_max = atof(optarg)*1024;
                break;
        }
    }
}

void recursive(char* name){
    DIR* d = opendir(name);
    struct dirent *ptr;
    struct stat buf;
    if(!d)
      return;
    while((ptr = readdir(d))!=NULL){
      char s[2048];
      if(ptr->d_type == 4){
        //directory
        sprintf(s,"%s/%s",name,ptr->d_name);
        stat(s,&buf);
        if(w+x+y+z == 0){
          printf("%s %llu %lf MB\n",s,buf.st_ino,buf.st_size/(double)1024);
        }
        if(!(strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0)){
          if(((w==1 && buf.st_ino == inode) || w == 0) && ((x==1 && strcmp(filename,ptr->d_name)==0)|| x==0) &&((y==1 && buf.st_size >= size_min)|| y==0) &&( (z == 1 && buf.st_size <= size_max)|| z==0)){
            printf("%s %llu %lf MB\n",s,buf.st_ino,buf.st_size/(double)1024);
          }
          recursive(s);
        }
      }
      else{
        //regular file
        sprintf(s,"%s/%s",name,ptr->d_name);
        stat(s,&buf);
        if((w+x+y+z == 0)||(((w==1 && buf.st_ino == inode) || w == 0) && ((x==1 && strcmp(filename,ptr->d_name)==0)|| x==0) &&((y==1 && buf.st_size >= size_min)|| y==0) &&( (z == 1 && buf.st_size <= size_max)|| z==0))){
          printf("%s %llu %lf MB\n",s,buf.st_ino,buf.st_size/(double)1024);
        }
      }
    }
    closedir(d);
}

int main(int argc, char **argv){
    parse_arg(argc,argv);
    recursive(pathname);
    return 0;
}
