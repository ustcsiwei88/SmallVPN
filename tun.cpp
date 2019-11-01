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

//#include<bits/stdc++.h>
//using namespace std;

char buff[2048];

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

void server(int fd){
	// fd: tun fd
	// sockfd: socket fd
	// connfd: connection fd
	int sockfd, connfd;
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
	int pos=0;
	int sz;
	while(1){
		sz = read(connfd, buff, 2048 - pos);
		if(sz==0){
			printf("connection ended\n");
			break;
		}
		write(fd, buff+pos, sz);
		pos+=sz;
		if(pos==2048) pos=0;
		printf("read %d %c",sz, buff[0]);
	}
}
void client(){
	//socket();
	//connect();
	//

}
int main(int argc, char* argv[]){
	//cout<<argc<<endl;
	int fd = alloc();
	server(fd);
	return 0;
}
