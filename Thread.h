//
// Created by Curie on 2018/10/1.
//

#include <stdbool.h>

#define TASK_DEFAULT_SIZE (1024*8*16)
#define THREAD_DEFAULT_INTERVAL 1000
typedef void (*Task)(void *);

enum THREAD_STATE_INNER;
typedef enum THREAD_STATE_INNER  THREAD_STATE;

struct BKThreadInner;
typedef struct  BKThreadInner BKThread;


void initEnv();
BKThread * newThread(Task task,void*arg);
void runThread(BKThread * thread);
int getThreadId();
void sleepThread(int time);
bool exist();
void terminalThread();
void* getPara();



