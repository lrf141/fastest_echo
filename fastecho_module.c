#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <linux/types.h>
#include <net/sock.h>

#include "fastecho.h"


#define DEFAULT_PORT 8888
#define DEFAULT_BACKLOG 128


//set module info
MODULE_DESCRIPTION("The fastest echo in kernel module");
MODULE_AUTHOR("lrf141");
MODULE_LICENSE("MIT");

//set module param
module_param(DEFAULT_PORT, int, S_IRUGO);
module_param(DEFAULT_BACKLOG, int, S_IRUGO);


struct socket *sock;
struct task_struct *echo_server;


static int open_listen(struct socket **);
static void close_listen(struct socket *);

static int fastecho_init_module(void){

  printk("Fastest Echo Server Start!!");

  return 0;
}


static void fastecho_cleanup_module(void){


  printk("Fastest Echo Server is unloaded!");

}


module_init(fastecho_init_module);
module_exit(fastecho_cleanup_module);
