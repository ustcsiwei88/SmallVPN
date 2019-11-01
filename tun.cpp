#include<linux/if.h>
#include<linux/if_tun.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/un.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<pthread.h>

//#include<bits/stdc++.h>
//using namespace std;

char rdbuff[2048];
char wrbuff[2048];

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
	//while(1);
	return fd;
}
void* writer(void* argument){
	int pos=0;
	int sz;
	while(1){
		sz = read(tunfd, wrbuff+pos, 2048-pos);
		if(sz<=0){
			break;
		}
		int sz2 = write(connfd, wrbuff + pos, sz);
		pos+=sz;
		printf("read %d from tun\n", sz);
		printf("write %d to socket\n", sz);
		if(pos==2048) pos=0;
	}
}
void server(){
	// fd: tun fd
	// sockfd: socket fd
	// connfd: connection fd
	int sockfd;// connfd;
	unsigned int len;
	struct sockaddr_in sa, ta;
	if((sockfd=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0){
		printf("Socket failed\n");
		exit(1);
	}
	memset(&sa, 0, sizeof(sa));
	sa.sin_family=AF_INET;
	sa.sin_port=htons(10010);
	sa.sin_addr.s_addr=htonl(INADDR_ANY);
	if(bind(sockfd, (struct sockaddr*)&sa, sizeof(sa))<0){
		printf("bind error\n");
		exit(1);
	}
	if(listen(sockfd, 10)<0){
		printf("listen error\n");
		exit(1);
	}
	if((connfd = accept(sockfd, (struct sockaddr *)&ta, &len))<0){
		printf("Connection error\n");
		close(sockfd);
		exit(1);
	}
	printf("Connection established\n");
	pthread_t th;
	pthread_create(&th, NULL, writer, NULL);
	int pos=0;
	int sz;
	while(1){
		sz = read(connfd, rdbuff+pos, 2048 - pos);
		if(sz==0){
			printf("Connection terminated\n");
			close(connfd);
			close(sockfd);
			break;
		}
		printf("receive sized %d\n", sz);
		write(tunfd, rdbuff+pos, sz);
		pos+=sz;
		if(pos==2048) pos=0;
		printf("read %d %c",sz, rdbuff[0]);
	}
}
void client(const char * ipaddr){
	// fd: tun fd
	// sockfd: socket fd
	// connfd: connection fd
	int sockfd;// connfd;
	unsigned int len;
	struct sockaddr_in sa, ta;
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
	printf("Connection established\n");
	pthread_t th;
	pthread_create(&th, NULL, writer, NULL);
	int pos=0;
	int sz;
	while(1){
		sz = read(connfd, rdbuff+pos, 2048 - pos);
		if(sz<=0){
			printf("connection terminated\n");
			close(connfd);
			close(sockfd);
			break;
		}
		write(tunfd, rdbuff+pos, sz);
		printf("received sized %d\n", sz);
		pos+=sz;
		if(pos==2048) pos=0;
		//printf("read %d %c",sz, rdbuff[0]);
	}
}
int main(int argc, char* argv[]){
	//cout<<argc<<endl;
	if(argc<2 || argc==2 && argv[1][0] == '1'){
		printf("Argument: 1 (client) server ip addr or 0 (server)\n");
		exit(1);
	}
	tunfd = alloc();
	//printf("%d, %s", argc, argv[1]);
	if(argv[1][0]=='0'){
		printf("running server now\n");
		server();
	}else if(argv[1][0]=='1'){
		printf("running client now\n");
		client(argv[2]);
	}
	return 0;
}
