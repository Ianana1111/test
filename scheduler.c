#include "threadtools.h"
#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * Print out the signal you received.
 * If SIGALRM is received, reset the alarm here.
 * This function should not return. Instead, call siglongjmp(sched_buf, 1).
 */

void sighandler(int signo) {
    // TODO
    if (signo == SIGTSTP) {
        printf("caught SIGTSTP\n");
        signal(SIGTSTP, sighandler);
        sigprocmask(SIG_BLOCK, &base_mask, NULL);
        longjmp(sched_buf, 1);
    }

    if (signo == SIGALRM) {
        printf("caught SIGALRM\n");
        alarm(timeslice);
        signal(SIGALRM, sighandler);
        sigprocmask(SIG_BLOCK, &base_mask, NULL);
        longjmp(sched_buf, 1);
    }
}

/*
 * Prior to calling this function, both SIGTSTP and SIGALRM should be blocked.
 */
void scheduler() {
    // TODO
    int recv = setjmp(sched_buf);
    
    if(recv != 0){

        if(bank.lock_owner == -1 && wq_size != 0){
            ready_queue[rq_size] = waiting_queue[0];
            bank.lock_owner = ready_queue[rq_size]->id;            
            rq_size++;
            wq_size--;

            for(int i = 0; i < wq_size; i++)
                waiting_queue[i] = waiting_queue[i+1];
        }

        else if(bank.lock_owner != -1 && RUNNING->id != bank.lock_owner){
            waiting_queue[wq_size] = ready_queue[rq_current];
            if(rq_current == rq_size-1)
                rq_current = 0;
            else
                ready_queue[rq_current] = ready_queue[rq_size - 1]; 

            wq_size++;
            rq_size--;
        }

        if(recv == 1){
            rq_current+= 1;
            rq_current %= rq_size;
            longjmp(RUNNING->environment, 1);
        }

        if(recv == 2){
            if(rq_current == rq_size - 1)
                rq_current = 0;            
        }

        if(recv == 3){
            free(ready_queue[rq_current]);
           
            if(rq_current == rq_size - 1)
                rq_current = 0;           
            else
                ready_queue[rq_current] = ready_queue[rq_size - 1]; 
            
            rq_size--;
        }
    }

    while(rq_size != 0)
        longjmp(ready_queue[rq_current]->environment, 1);
}