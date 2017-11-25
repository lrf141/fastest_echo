#include <linux/module.h>
#include <linux/kernel.h>

#include "thread.h"


//module info
MODULE_DESCRIPTION("The fastest echo server in kernel module");
MODULE_AUTHOR("lrf141");
MODULE_LICENSE("MIT");


static int fastecho_init_module(void){

        printk("Fastest Echo Server Start!!");

        //make kernel thread
        task = kthread_create(kthread_cb, NULL, "lrf141:fastecho");
        
        printk("[%s] wake up as kthread\n", task->comm);

        //launch task
        wake_up_process(task);
        
        return 0;
}


static void fastecho_cleanup_module(void){

        printk("Fastest Echo Server is unloaded!");
        printk("[%s] stop kthread\n", task->comm);
        kthread_stop(task);

}


module_init(fastecho_init_module);
module_exit(fastecho_cleanup_module);