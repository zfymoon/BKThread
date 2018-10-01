//
// Created by Curie on 2018/10/1.
//
#include "Thread.h"

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

enum THREAD_STATE_INNER{
    READY = 1,
    RUNNING ,
    WAIT,
    SLEEP,
    EXIT
};

enum THREAD_PRIORITY{
    LOW = 1,
    NORMAL,
    HIGH
};


struct BKThreadInner{
    jmp_buf env;
    char* stackPtr;
    THREAD_STATE state;
    enum THREAD_PRIORITY priority;
    BKThread* next;
    int id;
    Task task;
    void *arg;
    void *result;
    useconds_t totalTime;
    clock_t resumeTime;
};

struct BKThreadEnv{
    BKThread * currentThread;
    BKThread * mainThread;
    int totalThreadNum;
    useconds_t interval;
    bool  exist;
} ;
void schedule();
struct BKThreadEnv ThreadEnv;


void initEnv(){
    BKThread *mainThread;
    signal(SIGALRM,schedule);
    mainThread = (BKThread *)malloc(sizeof(BKThread));
    mainThread->stackPtr = NULL;
    mainThread->state = RUNNING;
    mainThread->priority = 1;
    mainThread->totalTime = 0;
    mainThread->next = mainThread;
    mainThread->id = 0;
    mainThread->task = NULL;
    mainThread->arg = NULL;

    ThreadEnv.currentThread = mainThread;
    ThreadEnv.mainThread = mainThread;
    ThreadEnv.totalThreadNum = 1;
}

BKThread * newThread(Task task,void*arg){
    BKThread *thread;
    thread = (BKThread *)malloc(sizeof(BKThread));
    thread->stackPtr = (char*)malloc(TASK_DEFAULT_SIZE);
    thread->state = READY;
    thread->priority = 1;
    thread->totalTime = 0;
    thread->id = ThreadEnv.totalThreadNum;
    thread->task = task;
    thread->arg = arg;

    ThreadEnv.totalThreadNum++;

    thread->next = ThreadEnv.mainThread->next;
    ThreadEnv.mainThread->next = thread;
    return thread;
}

void runThread(BKThread * thread){
    if(sigsetjmp(thread->env,SIGALRM) == 0){
        thread->state = RUNNING;
        if(sigsetjmp(ThreadEnv.currentThread->env,SIGALRM) == 0){
            ThreadEnv.currentThread = thread;
            siglongjmp(thread->env,1);
        } else{
            return;
        }
    } else{

    #ifdef _X86
        __asm__("mov %0, %%esp;": :"r"(ThreadEnv.currentThread->stackPtr + TASK_DEFAULT_SIZE) );
    #else
        __asm__("mov %0, %%rsp;": :"r"(ThreadEnv.currentThread->stackPtr + TASK_DEFAULT_SIZE) );
    #endif
        ThreadEnv.currentThread->task(ThreadEnv.currentThread->arg);
        ThreadEnv.currentThread->state = EXIT;
        ualarm(0,0);
        raise(SIGALRM);
    }
}

void schedule(){
    if(sigsetjmp(ThreadEnv.currentThread->env,SIGALRM) == 0){
        ThreadEnv.currentThread->totalTime += ThreadEnv.interval;
        do{
            ThreadEnv.currentThread = ThreadEnv.currentThread->next;
            switch (ThreadEnv.currentThread->state){
                case RUNNING:
                    break;
                case WAIT:
                    break;
                case SLEEP:
                    if(clock() > ThreadEnv.currentThread->resumeTime){
                        ThreadEnv.currentThread->state = RUNNING;
                    }
                    break;
            }
        }while (ThreadEnv.currentThread->state != RUNNING);

        ThreadEnv.interval = THREAD_DEFAULT_INTERVAL;
        switch (ThreadEnv.currentThread->priority){
            case LOW:
                ThreadEnv.interval *= 0.8;
                break;
            case NORMAL:
                break;
            case HIGH:
                ThreadEnv.interval *= 1.2;
        }
        ualarm(ThreadEnv.interval,0);
        siglongjmp(ThreadEnv.currentThread->env,1);
    }
}

int getThreadId(){
    return ThreadEnv.currentThread->id;
}
void sleepThread(int time){
    ThreadEnv.interval -= ualarm(0,0);
    ThreadEnv.currentThread->state = SLEEP;
    ThreadEnv.currentThread->resumeTime = clock() + time*CLOCKS_PER_SEC;
    raise(SIGALRM);
}
bool exist(){
    return ThreadEnv.exist;
};
void terminalThread(){
    ThreadEnv.exist = true;
}
void* getPara(){
    return ThreadEnv.currentThread->arg;
}
