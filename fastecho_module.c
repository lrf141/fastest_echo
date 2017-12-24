#include <linux/module.h>
#include <linux/kernel.h>

#include "thread.h"
#include "network.h"


//module info
MODULE_DESCRIPTION("The fastest echo server in kernel module");
MODULE_AUTHOR("lrf141");
MODULE_LICENSE("MIT");

static int fastecho_init_module(void){

        printk("Fastest Echo Server Start!!");

        //make socket
        int res;
        res = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &server);
        if(res < 0)
          return res;

        server->sk->sk_reuse = 1;
        
        socket_in.sin_family = AF_INET;
        socket_in.sin_addr.s_addr = htonl(INADDR_ANY);
        socket_in.sin_port = htons(DEFAULT_PORT);

        res = server->ops->bind(server, (struct sockaddr *)&socket_in, sizeof(socket_in));

        

        //make kernel thread
        task = kthread_create(kthread_cb, NULL, "lrf141:fastecho");
        
        printk("[%s] wake up as kthread\n", task->comm);

        //launch task as kernel process
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
