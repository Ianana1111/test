#include "threadtools.h"
#include <sys/signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void fibonacci(int id, int arg) {
    thread_setup(id, arg);

    for (RUNNING->i = 1; ; RUNNING->i++) {
        if (RUNNING->i <= 2)
            RUNNING->x = RUNNING->y = 1;
        else {
            /* We don't need to save tmp, so it's safe to declare it here. */
            int tmp = RUNNING->y;
            RUNNING->y = RUNNING->x + RUNNING->y;
            RUNNING->x = tmp;
        }
        printf("%d %d\n", RUNNING->id, RUNNING->y);
        sleep(1);

        if (RUNNING->i == RUNNING->arg) {
            thread_exit();
        }
        else {
            thread_yield();
        }
    }
}

void factorial(int id, int arg){
    // TODO
    thread_setup(id, arg);

    while(1) {

        RUNNING->i++;

        if (RUNNING->i == 1){
            RUNNING->y = 1;
        }

        RUNNING->y *= RUNNING->i;

        printf("%d %d\n", RUNNING->id, RUNNING->y);
        sleep(1);

        if (RUNNING->i >= RUNNING->arg) {
            thread_exit();
        }
        else {
            thread_yield();
        }
    }
}


void bank_operation(int id, int arg) {
    // TODO
    thread_setup(id, arg);

    if(RUNNING->i == 0){
        lock();
        
        if(bank.lock_owner == RUNNING->id)
            printf("%d acquired the lock\n", RUNNING->id);

        sleep(1);
        RUNNING->i = 1;
        thread_yield();
    } 

    if(RUNNING->i == 1){
        int tmp_balance = bank.balance;

        if(RUNNING->arg > 0)
            bank.balance += RUNNING->arg;

        else if(RUNNING->arg < 0 && bank.balance + RUNNING->arg >= 0)
            bank.balance += RUNNING->arg;

        printf("%d %d %d\n", RUNNING->id, tmp_balance, bank.balance);

        sleep(1);
        RUNNING->i = 2;
        thread_yield();
    }

    if (RUNNING->i == 2) {
        unlock();
        printf("%d released the lock\n", RUNNING->id);

        sleep(1);
        thread_exit();
    }

}