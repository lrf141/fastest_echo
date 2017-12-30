#include <linux/module.h>
#include <linux/kernel.h>

//#include "network.h"
#include "test_network.h"


//module info
MODULE_DESCRIPTION("The fastest echo server_sock in kernel module");
MODULE_AUTHOR("lrf141");
MODULE_LICENSE("MIT");

static int fastecho_init_module(void){

  printk("Fastest Echo Server Start!!");

  //kernel_ntw = kmalloc(sizeof(struct kernel_tcp),GFP_KERNEL);
  //start(); 
  run();
  return 0;
}


static void fastecho_cleanup_module(void){


  /*
  if(kernel_ntw->thread != NULL){
    kthread_stop(kernel_ntw->thread);
    kthread_stop(kernel_ntw->accept);
  }

  if(kernel_ntw->server_sock != NULL){
    sock_release(kernel_ntw->server_sock);
    kernel_ntw->server_sock = NULL;
  }

  kfree(kernel_ntw);
  kernel_ntw = NULL;
  */

  printk("Fastest Echo Server is unloaded!");

}


module_init(fastecho_init_module);
module_exit(fastecho_cleanup_module);
