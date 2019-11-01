#include<linux/if.h>
#include<linux/if_tun.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>
#include<bits/stdc++.h>

using namespace std;

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

int main(int argc, char* argv[]){
	//cout<<argc<<endl;
	alloc();
	return 0;
}
