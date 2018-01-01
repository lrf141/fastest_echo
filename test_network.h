#ifndef TEST_NETWORK_H
#define TEST_NETWORK_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/net.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/kthread.h>
#include <linux/slab.h>

#include <net/sock.h>
#include <net/tcp.h>
#include <net/inet_connection_sock.h>
#include <net/request_sock.h>

struct socket *sock0;
struct socket *sock;

extern int run(void);

int run(){
  
  int error;
  struct sockaddr_in sin;

  error = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock0);
  if(error != 0){
    printk("create socket error\n");
    return -1;
  }

  //sock0->sk->sk_reuse = 1;

  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_family = PF_INET;
  sin.sin_port = htons(8888);

  error = sock0->ops->bind(sock0, (struct sockaddr *)&sin, sizeof(sin));
  if(error < 0){
    printk("bind address error\n");
    return -1;
  }

  error = sock0->ops->listen(sock0, 5);
  if(error < 0){
    printk("listen error\n");
    return -1;
  }

  sock = (struct socket *)kmalloc(sizeof(struct socket), GFP_KERNEL);
  error = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
  if(error < 0){
    printk("csock create error\n");
    return -1;
  }

  //add queue
  DECLARE_WAITQUEUE(wait, current);
  struct inet_connection_sock *isock = inet_csk(sock->sk);

  while(1){


    if(reqsk_queue_empty(&isock->icsk_accept_queue)){
      add_wait_queue(&sock0->sk->sk_wq->wait, &wait);
      __set_current_state(TASK_INTERRUPTIBLE);
      schedule_timeout(HZ);
      __set_current_state(TASK_RUNNING);
      remove_wait_queue(&sock0->sk->sk_wq->wait, &wait);
      continue;
    }

    error = sock0->ops->accept(sock0, sock, O_NONBLOCK);
    
    if(error < 0){
      printk("accept error\n");
      sock_release(sock);
      sock_release(sock0);
      return -1;
    }else{
      printk("accept!!\n");
    }

  }
}

#endif
