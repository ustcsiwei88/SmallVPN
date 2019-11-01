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
		sz = read(tunfd, wrbuff, 2048-pos);
		if(sz==0) break;
		write(connfd, wrbuff + pos, sz);
		pos+=sz;
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
	if((sockfd=socket(AF_INET, SOCK_STREAM, 0))<0){
		printf("Socket failed");
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
	connfd = accept(sockfd, (struct sockaddr *)&ta, &len);
	printf("Connection established\n");
	pthread_t th;
	pthread_create(&th, NULL, writer, NULL);
	int pos=0;
	int sz;
	while(1){
		sz = read(connfd, rdbuff, 1 - pos);
		if(sz==0){
			printf("connection ended\n");
			break;
		}
		write(tunfd, rdbuff+pos, sz);
		pos+=sz;
		if(pos==1) pos=0;
		printf("read %d %c",sz, rdbuff[0]);
	}
}
void client(){
	// fd: tun fd
	// sockfd: socket fd
	// connfd: connection fd
	int sockfd;// connfd;
	unsigned int len;
	struct sockaddr_in sa, ta;
	if((sockfd=socket(AF_INET, SOCK_STREAM, 0))<0){
		printf("Socket failed");
		exit(1);
	}
	memset(&sa, 0, sizeof(sa));
	sa.sin_family=AF_INET;
	sa.sin_port=htons(10010);
	//sa.sin_addr.s_addr=htonl(INADDR_ANY);
	inet_pton(AF_INET, "127.0.0.1", &sa.sin_addr);
	
	connfd = connect(sockfd, (struct sockaddr *)&ta, sizeof(sa));
	printf("Connection established\n");
	pthread_t th;
	pthread_create(&th, NULL, writer, NULL);
	int pos=0;
	int sz;
	while(1){
		sz = read(connfd, rdbuff, 1 - pos);
		if(sz==0){
			printf("connection ended\n");
			break;
		}
		write(tunfd, rdbuff+pos, sz);
		pos+=sz;
		if(pos==1) pos=0;
		printf("read %d %c",sz, rdbuff[0]);
	}
}
int main(int argc, char* argv[]){
	//cout<<argc<<endl;
	if(argc<2){
		printf("argument: 1 (client) 0 (server)\n");
		exit(1);
	}
	tunfd = alloc();
	printf("%d, %s", argc, argv[1]);
	if(argv[1][0]=='0'){
		printf("running server now");
		server();
	}else if(argv[1][0]=='1'){
		printf("running client now");
		client();
	}
	return 0;
}
