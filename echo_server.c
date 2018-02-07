#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/tcp.h>
#include "fastecho.h"

#define BUF_SIZE 4096

static int echo_server_worker(void *arg){

  struct socket *sock;
  unsigned char *buf;

  sock = (struct socket *)arg;
  allow_signal(SIGKILL);
  allow_signal(SIGTERM);
  
  buf = kmalloc(BUF_SIZE, GFP_KERNEL);
  if(!buf){
    printk(KERN_ERR MODULE_NAME ": kmalloc error....\n");
    return -1;
  }
  
  return 0;
}



int echo_server_daemon(void *arg){

  struct echo_server_status *status;
  struct socket *sock;
  struct task_struct *thread;
  int error;

  //init
  status = (struct echo_server_status *)arg;
  allow_signal(SIGKILL);
  allow_signal(SIGTERM);

  while(!kthread_should_stop()){
    
    //using BLOCKING I/O
    error = kernel_accept(status->accept_sock, &sock, 0);
    if(error < 0){
      if(signal_pending(current))
        break;
      printk(KERN_ERR MODULE_NAME ": socket accept error = %d\n", error);
      continue;
    }

    //start server worker
    thread = kthread_run(echo_server_worker, sock, MODULE_NAME);
    if(IS_ERR(thread)){
      printk(KERN_ERR MODULE_NAME ": create worker thread error = %d\n", error);
      continue;
    }

  }


  return 0;
}
