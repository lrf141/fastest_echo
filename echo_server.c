#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/tcp.h>
#include "fastecho.h"

#define BUF_SIZE 4096

static int get_request(struct socket *sock, unsigned char *buf, size_t size){

  mm_segment_t oldfs;
  struct msghdr msg;
  struct kvec vec;
  int length;

  //kvec setting
  vec.iov_len = size;
  vec.iov_base = buf;

  //msghdr setting
  msg.msg_name = 0;
  msg.msg_namelen = 0;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;

  oldfs = get_fs();
  set_fs(KERNEL_DS);


  printk(MODULE_NAME ": start get response\n");
  //get msg
  length = kernel_recvmsg(sock, &msg, &vec, size, size, msg.msg_flags);

  set_fs(oldfs);

  printk(MODULE_NAME ": get request = %s\n", buf);

  return length;
}

static int send_request(struct socket *sock, unsigned char *buf, size_t size){

  mm_segment_t oldfs;
  int length;
  struct kvec vec; 
  struct msghdr msg;


  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;

  oldfs = get_fs();
  set_fs(KERNEL_DS);
  
  vec.iov_base = buf;
  vec.iov_len = strlen(buf);

  printk(MODULE_NAME ": start send request.\n"); 
  
  length = kernel_sendmsg(sock, &msg, &vec, 1, strlen(buf)-1);
  
  printk(MODULE_NAME ": send request = %s\n", buf);
  set_fs(oldfs);

  return length;
}

static int echo_server_worker(void *arg){

  struct socket *sock;
  unsigned char *buf;
  int res;

  sock = (struct socket *)arg;
  allow_signal(SIGKILL);
  allow_signal(SIGTERM);
  
  buf = kmalloc(BUF_SIZE, GFP_KERNEL);
  if(!buf){
    printk(KERN_ERR MODULE_NAME ": kmalloc error....\n");
    return -1;
  }

  while(!kthread_should_stop()){
    res = get_request(sock, buf, BUF_SIZE-1);
    if(res <= 0){
      if(res){
        printk(KERN_ERR MODULE_NAME ": get request error = %d\n", res);
      }
      break;
    }

    res = send_request(sock, buf, strlen(buf));
    if(res < 0){
      printk(KERN_ERR MODULE_NAME ": send request error = %d\n", res);
      break;
    }
  }

  res = get_request(sock, buf, BUF_SIZE-1);
  res = send_request(sock, buf, strlen(buf));

  kernel_sock_shutdown(sock, SHUT_RDWR);
  sock_release(sock);
  kfree(buf);
  
  return 0;
}



int echo_server_daemon(void *arg){

  struct echo_server_param *param;
  struct socket *sock;
  struct task_struct *thread;
  int error;

  //init
  param = (struct echo_server_status *)arg;
  allow_signal(SIGKILL);
  allow_signal(SIGTERM);

  while(!kthread_should_stop()){
    
    //using BLOCKING I/O
    error = kernel_accept(param->listen_sock, &sock, 0);
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
