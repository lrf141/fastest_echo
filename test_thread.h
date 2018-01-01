#ifndef TEST_THREAD_H
#define TEST_THREAD_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>

struct task_struct *task;
extern static int accept_thread(void);

#endif
