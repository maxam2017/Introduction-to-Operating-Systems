#include <iostream>
#include <iomanip> 
#include <fstream>
#include <map>
#include <ctime>

#define _ ios::sync_with_stdio(0); cin.tie(0);

using namespace std;
int slot[1024]={0};

struct Node{
    Node* pre;
    Node* next;
    int ref;
};
Node* record[1024];

int main(){_
    fstream fin;
    string filename;
    cout<<"filename:";
    cin>>filename;
    //timer start
    clock_t begin = clock();
    fin.open(filename,ios::in);
    int *trace = new int [20000001];
    int s=0;
    while(fin>>trace[s++]);
    cout<<fixed<<setprecision(9);
    cout<<"FIFO---"<<endl;
    cout<<"size\tmiss\thit\tpage fault ratio"<<endl;
    for(int size=128;size<=1024;size*=2){
        map<int,int> hash;
        int hit=0,miss=0,sp=0;
        for(int i=0;i<s-1;i++){
            int ref = trace[i];
            map<int,int>::iterator it = hash.find(ref);
            if(it != hash.end()){
                hit++;
            }
            else{
                miss++;
                if(sp>=size){
                    hash.erase(slot[sp%size]);
                }
                hash[ref] = sp%size;
                slot[sp%size] = ref;
                sp++;
            }
        }
        cout<<size<<'\t'<<miss<<'\t'<<hit<<'\t'<<miss/(double)(hit+miss)<<endl;
    }
    cout<<"LRU---"<<endl;
    for(int size=128;size<=1024;size*=2){
        map<int,int> hash;
        int hit=0,miss=0,sp=0;
        Node *start=NULL,*last=NULL;
        for(int i=0;i<s-1;i++){
            int ref = trace[i];
            Node* ptr;
            map<int,int>::iterator it = hash.find(ref);
            if(it != hash.end()){
                ptr = record[hash[ref]];
                if(ptr!=last){
                    if(ptr==start){
                        start->next->pre = NULL;
                        start = ptr->next;
                    }
                    else if(last!=start){
                        if(ptr->pre)
                            ptr->pre->next = ptr->next;
                        if(ptr->next)
                            ptr->next->pre = ptr->pre;
                    }
                    last->next = ptr;
                    ptr->pre = last;
                    last = ptr;
                }
                hit++;
            }
            else{
                miss++;
                if(sp>=size){
                    int victim = start->ref;
                    ptr = start->next;
                    start = ptr;
                    ptr->pre = NULL;
                    int target = hash[victim];
                    hash.erase(victim);
                    hash[ref] = target;
                    //slot[target] = ref;
                    ptr = new Node();
                    ptr->next = NULL;
                    ptr->ref = ref;
                    ptr->pre = last;
                    last->next = ptr;
                    last = ptr;
                    record[target] = ptr;
                }
                else{
                    hash[ref] = sp;
                    //slot[sp] = ref;
                    ptr = new Node();
                    ptr->next = NULL;
                    ptr->ref = ref;
                    if(!sp){
                        start = ptr;
                        ptr->pre = NULL;
                        last = ptr;
                    }
                    else{
                        ptr->pre = last;
                        last->next = ptr;
                        last = ptr;
                    }
                    record[sp] = ptr;
                    sp++;
                }
            }
        }
        cout<<size<<'\t'<<miss<<'\t'<<hit<<'\t'<<miss/(long double)(hit+miss)<<endl;
    }
    //timer stop
    clock_t end = clock();
    cout<<"\nElapsed time: "<<double(end - begin) / CLOCKS_PER_SEC * 1000<<"ms"<<endl;
    delete [] trace;
}