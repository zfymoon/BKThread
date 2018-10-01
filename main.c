#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "Thread.h"

void fun(){
    int index = 10;
    while(index >= 0){
        index --;
        sleepThread(1);
        printf("In thread1: %d at %d\n", getThreadId(),index);
    }
    terminalThread();
}

void fun2(){
    int index = 10000;
    while(index >= 0){
        index --;
        printf("In thread2: %d at %d\n",getThreadId(),index);
    }
}

int main() {
    initEnv();
    BKThread *thread1 = newThread(fun,NULL);
    runThread(thread1);

    BKThread *thread2 = newThread(fun2,NULL);
    runThread(thread2);


    while (!exist()){

    }
    return 0;
}