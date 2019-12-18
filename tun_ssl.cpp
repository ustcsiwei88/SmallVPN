#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<pthread.h>
#include<linux/if.h>
#include<linux/if_tun.h>

#include<openssl/bio.h>
#include<openssl/ssl.h>
#include<openssl/err.h>

//#include<bits/stdc++.h>

using namespace std;
 
#define MAX_BUFF 2048

#define CA_FILE "certs/domain.crt"
#define CA_DIR "certs"
char rdbuff[MAX_BUFF];
char wrbuff[MAX_BUFF];

int connfd;//connection fd
int tunfd;

int PORT;

BIO *bio;
SSL *ssl = NULL;
SSL_CTX *ctx = NULL;

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
		*((int*)(wrbuff)) = read(tunfd, wrbuff + sizeof(int)/*+pos*/, MAX_BUFF - sizeof(int) /*-pos*/);
		if(*((int*)(wrbuff))==0){
			printf("writer terminated\n");
			break;
		}
		unsigned int sz2 = SSL_write(ssl, wrbuff/* + pos*/, *((int*)(wrbuff)) + sizeof(int));
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
	sa.sin_port=htons(PORT);
	sa.sin_addr.s_addr = INADDR_ANY;
	int on = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))){
		printf("setsockopt error\n");
		exit(1);
	}
	if(bind(sockfd, (struct sockaddr*)&sa, sizeof(sa))<0){
		printf("bind error\n");
		exit(1);
	}
	if(listen(sockfd, 1)<0){
		printf("listen error\n");
		exit(1);
	}
	if((connfd = accept(sockfd, (struct sockaddr *)&ta, &len))<0){
		printf("Connection error\n");
		close(sockfd);
		exit(1);
	}
	printf("Connection established, fd: %d\n", connfd);
	// SSL_CTX_set_verify(ctx, SSL_VERIFY_, NULL);

	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, connfd);
	int err;
	if((err=SSL_accept(ssl))<=0){
		err = SSL_get_error(ssl, err);
		printf("Hankshake failed %d\n", err);
		exit(1);
	}
	printf("SSL connection established\n");
	pthread_t th;
	pthread_create(&th, NULL, writer, NULL);
	int pos=0;
	int sz, sz2;
	while(1){
		// printf("Wanna receive stuffs\n");
		if(SSL_read(ssl, &sz, sizeof(int))==0){
			printf("Connection terminated\n");
			SSL_shutdown(ssl);
			SSL_free(ssl);
			close(connfd);
			close(sockfd);
			break;
		}
		// printf("readsth %d\n", sz);

		// printf("sz = %d\n", sz);
		pos=0;
		while(pos<sz){
			pos+=SSL_read(ssl, rdbuff + pos, sz-pos /*- pos*/);
		}
		// printf("receive sized %d\n", sz2);
		pos=0;
		while(pos<sz && pos>=0){
			pos+=write(tunfd, rdbuff + pos, sz-pos);
		}
		// printf("write to tun %d\n",sz2);
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
	sa.sin_port=htons(PORT);
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
	
	
	// SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
	ssl = SSL_new(ctx);
	SSL_set_fd(ssl, connfd);
	int err;
	if((err=SSL_connect(ssl))<=0){
		//SSL_get_error();
		err = SSL_get_error(ssl, err);
		printf("Hankshake failed in connect %d\n", err);
		exit(1);
	}
	printf("SSL connection established\n");
	pthread_t th;
	pthread_create(&th, NULL, writer, NULL);
	int pos=0;
	int sz;
	while(1){
		// printf("Wanna receive stuffs\n");
		if(SSL_read(ssl, &sz, sizeof(int))==0){
			printf("connection terminated\n");
			SSL_shutdown(ssl);
			SSL_free(ssl);
			close(connfd);
			close(sockfd);
			break;
		}
		//printf("readsth %d\n", sz);

		pos=0;
		// printf("sz = %d\n", sz);
		while(pos<sz){
			// printf("reading %d\n", pos);
			pos+=SSL_read(ssl, rdbuff+pos, sz-pos);
		}
		pos=0;
		while(pos<sz && pos>=0){
			// printf("HERE %d %d\n",sz, pos);
			pos+=write(tunfd, rdbuff+pos, sz-pos);
		}
		// printf("client write finished\n");
		// printf("received sized %d\n", sz);
		// pos+=sz;
		// if(pos==2048) pos=0;
		//printf("read %d %c",sz, rdbuff[0]);
	}
}

void init_openssl(){
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();
}

SSL_CTX *create_context(int typ){
	const SSL_METHOD *method;
	SSL_CTX * res;
	if(typ == 0) method = SSLv23_server_method();
	else method = SSLv23_client_method();
	ctx = SSL_CTX_new(method);
	if(!ctx){
		printf("Unable to create SSL context\n");
		exit(1);
	}
	if(SSL_CTX_set_cipher_list(ctx, "AES128-SHA")<=0){
		printf("Error in setting cypher list");
		exit(1);
	}
	if(typ == 0){
		if(SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM)<=0){
			printf("Using certificate file failed\n");
			exit(1);
		}
		if(SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM)<=0){
			printf("Using private key file failed\n");
			exit(1);
		}
		// if(SSL_CTX_load_and_set_client_)

	}else{
		if(SSL_CTX_use_certificate_file(ctx, "client.crt", SSL_FILETYPE_PEM)<=0){
			printf("Using certificate file failed\n");
			exit(1);
		}
		if(SSL_CTX_use_PrivateKey_file(ctx, "client.key", SSL_FILETYPE_PEM)<=0){
			printf("Using private key file failed\n");
			exit(1);
		}
	}
	if(SSL_CTX_check_private_key(ctx)==0){
		printf("Private key does not match certificate public\n");
		exit(1);
	}

	if(SSL_CTX_load_verify_locations(ctx, NULL, CA_DIR)<=0){
		printf("Error loading verify location\n");
		exit(1);
	}
	// if(typ==0) SSL_CTX_set_ecdh_auto(ctx, 1);
	return ctx;
}

void configure_context(SSL_CTX *ctx){
	// SSL_CTX_set_ecdh_auto(ctx, 1);
	// SSL_CTX_set_ecdh_auto(ctx, 1);

	// if(SSL_CTX_use_certificate_file(ctx, "domain.crt", SSL_FILETYPE_PEM)<=0){
	// 	printf("Using certificate file failed\n");
	// 	exit(1);
	// }
	// if(SSL_CTX_use_PrivateKey_file(ctx, "domain.key", SSL_FILETYPE_PEM)<=0){
	// 	printf("Using private key file failed\n");
	// 	exit(1);
	// }

	// if(SSL_CTX_check_private_key(ctx)==0){
	// 	printf("Private key does not match certificate public\n");
	// 	exit(1);
	// }
}

int main(int argc, char* argv[]){
	//cout<<argc<<endl;
	if(argc<3 || argc==3 && argv[1][0] == '1'){
		printf("Argument: 1 (client) server ip addr port or 0 port (server)\n");
		exit(1);
	}
	PORT = atoi(argv[2]);
	tunfd = alloc();
	
	init_openssl();
	ctx = create_context(argv[1][0]-'0');
	// if(argv[1][0]=='0') 
	configure_context(ctx);

	if(argv[1][0]=='0'){
		printf("running server now\n");
		server();
	}else if(argv[1][0]=='1'){
		printf("running client now\n");
		client(argv[3]);
	}
	SSL_CTX_free(ctx);
	EVP_cleanup();
	return 0;
}
