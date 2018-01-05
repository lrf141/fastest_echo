#include "test_network.h" 

struct socket *sock0;
struct socket *sock;

int run(){
  
  int error;
  struct sockaddr_in sin;

  error = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock0);
  if(error != 0){
    printk("create socket error\n");
    return -1;
  }

  //sock0->sk->sk_reuse = 1;

  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_family = AF_INET;
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
  error = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
  if(error < 0){
    printk("csock create error\n");
    return -1;
  }

  struct inet_connection_sock *isock = inet_csk(sock->sk);

  while(1){

    error = sock0->ops->accept(sock0, sock, O_NONBLOCK);
    
    if(error < 0){
      printk(KERN_ERR "accept error\n");
      sock_release(sock);
      sock_release(sock0);
      return -1;
    }else{
      printk("accept!!\n");
    }

  }
}
