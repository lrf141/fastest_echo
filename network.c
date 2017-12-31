#include "network.h"

int send_str(struct socket *sock, char *buf, int len){
  if(sock == NULL){
    printk("send csock is NULL\n");
    return -1;
  }

  struct msghdr msg;
  struct iovec iov;
  int size;

  iov.iov_base = buf;
  iov.iov_len = len;

  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;
  //msg.msg_iov = &iov;
  //msg.msg_iovlen = 1;
  msg.msg_name = 0;
  msg.msg_namelen = 0;

  printk("send str \n");
  size = sock_sendmsg(sock, &msg);
  printk("send str done\n");
  return size;
}

int receive(struct socket *sock, unsigned char *buf, int len){
  
  if(sock == NULL){
    printk("receive csock is NULL\n");
    return -1;
  }

  struct msghdr msg;
  struct iovec iov;
  int size = 0;

  //init iovec
  iov.iov_base = buf;
  iov.iov_len = len;

  //init msghdr
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;
  msg.msg_name = 0;
  msg.msg_namelen = 0;
  //msg.msg_iov = &iov;
  //msg.msg_iovlen = 1;

  size = sock_recvmsg(sock, &msg, msg.msg_flags);

  printk("echo server received msg\n");
  printk("result: %s\n", buf);
  
  return 0;
}

int accept_thread(){

  int error,res;
  struct socket *socket;
  struct socket *csock;

  unsigned char buf[MSGSIZE + 1];


  //init socket
  socket = kernel_ntw -> server_sock;
  csock = (struct socket*)kmalloc(sizeof(struct socket), GFP_KERNEL);
  
  //socket create
  error = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &csock);
  if(error < 0){
    printk("create csocket error\n");
    return -1;
  }

  printk("accept_thread the csock is :%d,%d\n",csock,kernel_ntw->server_sock);

  //accept
  //あとでループ無しの場合をやってみる
  //while(kernel_ntw->running == 1){
    
    printk("accept start\n");
    res = socket->ops->accept(socket, csock, O_NONBLOCK);
    if(res < 0){
      printk("accept error\n");
      sock_release(csock);
      return res;
    }

    //receive msg
    memset(&buf, 0, MSGSIZE+1);
    printk("receive start\n");

    while(receive(csock, buf, MSGSIZE)){
      memset(&buf, 0, MSGSIZE+1);
      strcat(buf, "testing... ");
      printk("do receive and send\n");
      send_str(csock, buf, strlen(buf));
    }

    
  //}

  return res;
}


int start_listen(){

  int error;
  struct socket *socket;
  struct sockaddr_in sin, sin_send; 

  kernel_ntw->running = 1;

  //socket create
  error = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &kernel_ntw->server_sock);
  if(error < 0){
    printk("create socket error\n");
    return -1;
  }

  socket = kernel_ntw->server_sock;
  kernel_ntw->server_sock->sk->sk_reuse = 1;

  //init sockaddr_in
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(DEFAULT_PORT);

  //bind
  error = socket->ops->bind(socket, (struct sockaddr*)&sin, sizeof(sin));
  if(error < 0){
    printk("bind address error\n");
    return -1;
  }

  //listen
  error = socket->ops->listen(socket, 5);
  if(error < 0){
    printk("listen error\n");
    return -1;
  }

  //start accept thread
  kernel_ntw->accept = kthread_run((void *)accept_thread, NULL, "lrf141:fastecho");

  return 0;
}

int start(){

  //update running flag
  kernel_ntw -> running = 1;

  //start tcp listen thread
  kernel_ntw -> thread = kthread_run((void *)start_listen, NULL, "lrf141:fastecho");
  return 0;
}

