#ifndef _NETWORK_H
#define _NETWORK_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/socket.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <net/sock.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/delay.h>
#include <linux/un.h>
#include <linux/unistd.h>
#include <linux/wait.h>
#include <linux/ctype.h>
#include <asm/unistd.h>
#include <net/sock.h>
#include <net/tcp.h>
#include <net/inet_connection_sock.h>
#include <net/request_sock.h>

#define DEFAULT_PORT 8888
#define CONNECT_PORT 23
#define MSGSIZE 1024

struct kernel_tcp{

  int running;
  struct socket *server_sock;
  struct task_struct *thread;
  struct task_struct *accept;

};

struct kernel_tcp *kernel_ntw;

extern int start_listen(void);
extern int start(void);
extern int accept_thread(void);
extern int receive(struct socket *, unsigned char *, int);
extern int send_str(struct socket *, char *, int);

#endif
