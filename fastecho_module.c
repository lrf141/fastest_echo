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

static int open_listen(struct socket **result){

  struct socket *sock;
  struct sockaddr_in addr;
  int error;
  int opt = 1;


  //using IPv4, TCP/IP
  error = sock_create_kern(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
  if(error < 0){
    printk(KERN_ERR MODULE_NAME ": socket create error = %d\n", error);
    return error;
  }

  
  //set tcp_nodelay
  error = kernel_setsockopt(sock, 
                            SOL_TCP, TCP_NODELAY,
                            (char *)&opt, sizeof(opt));
  if(error < 0){
    printk(KERN_ERR MODULE_NAME ": setsockopt tcp_nodelay setting error = %d\n", error);
    sock_release(sock);
    return error;
  }

  
  //setting sockaddr_in
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(DEFAULT_PORT);


  //bind
  error = kernel_bind(sock, (struct sockaddr *)&addr, sizeof(addr));
  if(error < 0){
    printk(KERN_ERR MODULE_NAME ": socket bind error = %d\n", error);
    sock_release(sock);
    return error;
  }


  //listen
  error = kenel_listen(sock, DEFAULT_BACKLOG);
  if(error < 0){
    printk(KERN_ERR MODULE_NAME ": socket bind error = %d\n", error);
    sock_release(sock);
    return error;
  }

  
  *result = sock;
  return 0;

}

static void close_listen(struct socket *sock){
  kernel_sock_shutdown(sock, SHUT_RDWR);
  sock_release(sock);
}


module_init(fastecho_init_module);
module_exit(fastecho_cleanup_module);
