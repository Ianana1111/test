#ifndef THREADTOOL
#define THREADTOOL
#include <setjmp.h>
#include <sys/signal.h>
#include "bank.h"


#define THREAD_MAX 16  // maximum number of threads created
#define BUF_SIZE 512
struct tcb {
    int id;  // the thread id
    jmp_buf environment;  // where the scheduler should jump to
    int arg;  // argument to the function
    int i, x, y;  // declare the variables you wish to keep between switches
};



extern int timeslice;
extern jmp_buf sched_buf;
extern struct tcb *ready_queue[THREAD_MAX], *waiting_queue[THREAD_MAX];
extern struct Bank bank;
/*
 * rq_size: size of the ready queue
 * rq_current: current thread in the ready queue
 * wq_size: size of the waiting queue
 */
extern int rq_size, rq_current, wq_size;
/*
* base_mask: blocks both SIGTSTP and SIGALRM
* tstp_mask: blocks only SIGTSTP
* alrm_mask: blocks only SIGALRM
*/
extern sigset_t base_mask, tstp_mask, alrm_mask;
/*
 * Use this to access the running thread.
 */
#define RUNNING (ready_queue[rq_current])

void sighandler(int signo);
void scheduler();

// TODO
#define thread_create(func, id, arg) {\
    func(id, arg);\
}


#define thread_setup(id, arg) {\
    printf("%d %s\n", id, _func_);\
    struct tcb current_thread = (struct tcb)malloc(sizeof(struct tcb));\
    current_thread->id = id;\
    current_thread->arg = arg;\
    if(setjmp(current_thread->environment) == 0){\
        ready_queue[rq_size] = current_thread;\
        rq_size++;\
        return;\
    }\
}
// in _funct_ we return

#define thread_exit() {\
    longjmp(sched_buf, 3);\
}


#define thread_yield() {\
    if(setjmp(ready_queue[rq_current]->environment) == 0){\
        sigprocmask(SIG_UNBLOCK, &tstp_mask, NULL);\
        sigprocmask(SIG_UNBLOCK, &alrm_mask, NULL);\
    }\
    sigprocmask(SIG_BLOCK, &base_mask, NULL);\
}


#define lock() {\
    if( bank.lock_owner == -1 || bank.lock_owner == RUNNING->id){\
        bank.lock_owner = RUNNING->id;\
    }\
    else{\
        if(setjmp(RUNNING->environment)==0)\
            longjmp(sched_buf, 2);\
    }\
}


#define unlock() {\
    if( bank.lock_owner == RUNNING->id)\
        bank.lock_owner = -1;\
}

#endif // THREADTOOL