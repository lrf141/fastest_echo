#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/unistd.h>
#include <net/sock.h>
#include <net/tcp.h>
#include <net/inet_connection_sock.h>
#include <net/request_sock.h>
#include <linux/wait.h>

#include "test_network.h" 

struct socket *sock;
struct socket *accept_sock;
struct inet_connection_sock *isock;
struct task_struct *accept_thread;

int server_listen(void);
int server_accept(void);
int echo_receive(unsigned char *, int);


int server_listen(void){

  struct sockaddr_in server;
  int error = 0;

  DECLARE_WAIT_QUEUE_HEAD(wq);
  allow_signal(SIGKILL);
  allow_signal(SIGTERM);

  //socket create
  error = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
  if(error < 0){
    printk(KERN_ERR "server socket create error:%d\n", error);
    return error;
  }

  printk("socket create ok.\n");


  memset(&server, 0, sizeof(server));

  //init socket setting
  server.sin_family = AF_INET;
  server.sin_port = htons(DEFAULT_PORT);
  server.sin_addr.s_addr = INADDR_ANY;

  sock->sk->sk_reuse = 1;


  //socket bind
  error = sock->ops->bind(sock, (struct sockaddr *)&server, sizeof(server));
  if(error < 0){
    printk(KERN_ERR "server socket bind error:%d\n", error);
    return error;
  }

  printk("socket bind ok.\n");



  //socket listen
  error = sock->ops->listen(sock, 8);
  if(error < 0){
    printk(KERN_ERR "socket listen error:%d\n", error);
    return error;
  }

  printk("socket listen ok.\n");

  accept_thread = kthread_run((void *)server_accept, NULL, "fastestecho");

  while(1){
    wait_event_timeout(wq, 0, 3*HZ);
    if(kthread_should_stop()){
      printk("echo server listen thread stop\n");
      return 0;
    }

    if(signal_pending(current)){
      goto release;
    }
  }

  sock_release(sock);

release:
  sock_release(sock);

  return 0;
}

int server_accept(void){

  int error = 0;
  unsigned char buf[1024];

  DECLARE_WAITQUEUE(accept_wait, current);
  allow_signal(SIGKILL);
  allow_signal(SIGSTOP);

  printk("start accept thread.\n");

  while(1){

    error = sock_create(sock->sk->sk_family, sock->type, sock->sk->sk_protocol, &accept_sock);
    if(error < 0 || !accept_sock){
      printk(KERN_ERR "accept socket create error:%d\n", error);
      return error;
    }


    accept_sock->type = sock->type;
    accept_sock->ops  = sock->ops;
    
    isock = inet_csk(sock->sk);
    
    
    add_wait_queue(&sock->sk->sk_wq->wait, &accept_wait);
    while(reqsk_queue_empty(&isock->icsk_accept_queue)){
      
      
      __set_current_state(TASK_INTERRUPTIBLE);
      schedule_timeout(HZ);

      
      if(kthread_should_stop()){
        printk("accept thread stop\n");
        __set_current_state(TASK_RUNNING);
        remove_wait_queue(&sock->sk->sk_wq->wait, &accept_wait);
        sock_release(accept_sock);
        return 0;
      }

      if(signal_pending(current)){
        __set_current_state(TASK_RUNNING);
        remove_wait_queue(&sock->sk->sk_wq->wait, &accept_wait);
        goto release;
      }

    }

    __set_current_state(TASK_RUNNING);
    remove_wait_queue(&sock->sk->sk_wq->wait, &accept_wait);

    error = sock->ops->accept(sock, accept_sock, O_NONBLOCK);
    if(error < 0){
      printk(KERN_ERR "accept error:%d\n", error);
      return error;
    }

    memset(&buf, 0, 1024);
    printk("start get msg\n");

    while(echo_receive(buf,1024)){
    }

    if(signal_pending(current))
      break;

  }
release:
  sock_release(accept_sock);


  return 0;
}

int echo_receive(unsigned char *buf, int len){
  printk("echo receive accept_sock is: %d, %d\n", (int)accept_sock, (int)sock);
  
  return 0;
}
