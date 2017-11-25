#ifndef _THREAD_H
#define _THREAD_H

#include <linux/kthread.h>
#include <linux/sched.h>

//linux kernel process
struct task_struct *task;


static int kthread_cb(void *arg){

    printk("[%s] running as kthread\n", task->comm);

    while(!kthread_should_stop()){
        schedule();
    }

    return 0;

}


#endif