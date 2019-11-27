#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/un.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<pthread.h>
#include<linux/if.h>
#include<linux/if_tun.h>

#include "encryption.h"

int connfd;//connection fd
int tunfd;

int alloc(){
	struct ifreq ifr;
	int fd, err;
	if((fd = open("/dev/net/tun", O_RDWR))<0){
		printf("Open tun failed\n");
		exit(1);
	}
	ifr.ifr_flags=IFF_TUN;
	strncpy(ifr.ifr_name, "tun0", IFNAMSIZ);
	if((err=ioctl(fd, TUNSETIFF, (void*)&ifr))<0){
		close(fd);
		exit(err);
	}
	return fd;
}

void build_server(){
	// fd: tun fd
	// sockfd: socket fd
	// connfd: connection fd
	int sockfd;// connfd;
	struct sockaddr_in sa, ta;
    char str[INET_ADDRSTRLEN];
    socklen_t ta_len = sizeof(ta);
	if((sockfd=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0){
		printf("Socket failed\n");
		exit(1);
	}
	memset(&sa, 0, sizeof(sa));
	sa.sin_family=AF_INET;
	sa.sin_port=htons(10010);
	sa.sin_addr.s_addr = INADDR_ANY;
	if(bind(sockfd, (struct sockaddr*)&sa, sizeof(sa))<0){
		printf("bind error\n");
		exit(1);
	}
	if(listen(sockfd, 10)<0){
		printf("listen error\n");
		exit(1);
	}

    struct pollfd fdset[2];
    memset(&fdset, 0, sizeof(fdset));
    fdset[0].fd = tunfd;
    fdset[0].events = POLLIN;

    ssl_init("server.crt", "server.key");
    while(1){

        if((connfd = accept(sockfd, (struct sockaddr *)&ta, &ta_len))<0){
            printf("Connection error\n");
            close(sockfd);
            exit(1);
        }
        ssl_client_init(&client, connfd, SSLMODE_SERVER);
        fdset[1].fd = connfd;
        fdset[1].events = POLLERR | POLLHUP | POLLNVAL | POLLIN;
#ifdef POLLRDHUP
        fdset[1].events |= POLLRDHUP;
#endif
        inet_ntop(ta.sin_family, &ta.sin_addr, str, INET_ADDRSTRLEN);
        printf("Connection established from %s:%d\n", str, ntohs(ta.sin_port));
        while(1){
            fdset[1].events &= ~POLLOUT;
            fdset[1].events |= (ssl_client_want_write(&client)? POLLOUT : 0);
			int nready = poll(&fdset[0], 2, -1);

            if (nready == 0)
                continue; /* no fd ready */

            int revents = fdset[1].revents;
            if (revents & POLLIN)
                if (do_sock_read() == -1)
                break;
            if (revents & POLLOUT)
                if (do_sock_write() == -1)
                break;
            if (revents & (POLLERR | POLLHUP | POLLNVAL))
                break;
#ifdef POLLRDHUP
            if (revents & POLLRDHUP)
                break;
#endif
            if (fdset[0].revents & POLLIN)
                do_read(connfd);

            if (client.encrypt_len>0)
                do_encrypt();
            
        }
        close(fdset[1].fd);
        ssl_client_cleanup(&client);
    }
}

void build_client(const char * ipaddr){
	// fd: tun fd
	// sockfd: socket fd
	// connfd: connection fd
	int sockfd;// connfd;
	struct sockaddr_in sa;
	if((sockfd=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0){
		printf("Socket failed");
		exit(1);
	}
	memset(&sa, 0, sizeof(sa));
	sa.sin_family=AF_INET;
	sa.sin_port=htons(10010);
	//sa.sin_addr.s_addr=htonl(INADDR_ANY);
	inet_pton(AF_INET, ipaddr, &sa.sin_addr);
	printf("Connecting to %s ...\n", ipaddr);
	connfd = connect(sockfd, (struct sockaddr *)&sa, sizeof(sa));
	if(connfd<0){
		printf("Connection error\n");
		close(sockfd);
		exit(1);
	}
	connfd = sockfd;
	printf("Connection established, fd: %d\n", connfd);
	struct pollfd fdset[2];
	memset(&fdset, 0, sizeof(fdset));
	fdset[0].fd = tunfd;
	fdset[0].events = POLLIN;
	ssl_init(0,0);
    ssl_client_init(&client, sockfd, SSLMODE_CLIENT);
	fdset[1].fd = sockfd;
    fdset[1].events = POLLERR | POLLHUP | POLLNVAL | POLLIN;
#ifdef POLLRDHUP
    fdset[1].events |= POLLRDHUP;
#endif
	do_ssl_handshake();

	while(1){
		fdset[1].events &= ~POLLOUT;
        fdset[1].events |= ssl_client_want_write(&client)? POLLOUT:0;

        int nready = poll(&fdset[0], 2, -1);
		if (nready == 0)
        	continue; 

        int revents = fdset[1].revents;
        if (revents & POLLIN)
        if (do_sock_read() == -1)
            break;
        if (revents & POLLOUT)
        if (do_sock_write() == -1)
            break;
        if (revents & (POLLERR | POLLHUP | POLLNVAL))
        	break;
#ifdef POLLRDHUP
        if (revents & POLLRDHUP)
        	break;
#endif
        if (fdset[0].revents & POLLIN)
        	do_read(tunfd);
        if (client.encrypt_len>0)
        	do_encrypt();
	}
    close(fdset[1].fd);
    ssl_client_cleanup(&client); 
}
int main(int argc, char* argv[]){
	//cout<<argc<<endl;
	if(argc<2 || (argc==2 && argv[1][0] == '1')){
		printf("Argument: 1 (client) server ip addr or 0 (server)\n");
		exit(1);
	}
	tunfd = alloc();
	if (tunfd<0){
		die("alloc()");
	}
	//printf("%d, %s", argc, argv[1]);
	if(argv[1][0]=='0'){
		printf("running server now\n");
		build_server();
	}else if(argv[1][0]=='1'){
		printf("running client now\n");
		build_client(argv[2]);
	}
	return 0;
}
