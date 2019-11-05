//#include <openssl/applink.c>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include<sys/socket.h>
#include<stdio.h>
#include<string.h>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <unistd.h> 

int sockfd, newsockfd;
SSL_CTX *sslctx;
SSL *cSSL;
char wrbuffer[2048];

void InitializeSSL()
{
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
}

void DestroySSL()
{
    ERR_free_strings();
    EVP_cleanup();
}

void ShutdownSSL()
{
    SSL_shutdown(cSSL);
    SSL_free(cSSL);
}

void UseSSL(int socketfd)
{
    sslctx = SSL_CTX_new( SSLv23_server_method());
    SSL_CTX_set_options(sslctx, SSL_OP_SINGLE_DH_USE);
    int use_cert = SSL_CTX_use_certificate_file(sslctx, "/serverCertificate.pem" , SSL_FILETYPE_PEM);

    int use_prv = SSL_CTX_use_PrivateKey_file(sslctx, "/serverCertificate.pem", SSL_FILETYPE_PEM);

    cSSL = SSL_new(sslctx);
    SSL_set_fd(cSSL, socketfd);
    //SSL Accept portion. All reads and writes must use SSL
    int ssl_err = SSL_accept(cSSL);
    if(ssl_err <= 0)
    {
        ShutdownSSL();
    }
}

void server()
{
    InitializeSSL();
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd< 0)
    {
		printf("Socket failed\n");
		exit(1);
    }
    struct sockaddr_in sa, ca; 
    unsigned int clilen;
    bzero((char *) &sa, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(8080);

    bind(sockfd, (struct sockaddr *) &sa, sizeof(sa));

    listen(sockfd,5);
    newsockfd = accept(sockfd, (struct sockaddr *) &ca, &clilen);
    UseSSL(newsockfd);
    SSL_write(cSSL, "Hello", strlen("Hello"));
    ShutdownSSL();
}

void client()
{
    printf("111");
    unsigned int clilen;
    InitializeSSL();
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd< 0)
    {
		printf("Socket failed\n");
		exit(1);
    }
    struct sockaddr_in sa, ca;
    bzero((char *) &sa, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(8080);

    if(inet_pton(AF_INET, "127.0.0.1", &sa.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        exit(1); 
    } 

    if (connect(sockfd, (struct sockaddr *)&sa, sizeof(sa)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        exit(1); 
    }

    newsockfd = accept(sockfd, (struct sockaddr *) &ca, &clilen);
    UseSSL(newsockfd);
    int res = SSL_write(cSSL, wrbuffer, sizeof(wrbuffer) - 1);
	printf("Got %d chars :%s\n", res, wrbuffer);
    ShutdownSSL();   
}

int main(int argc, char* argv[]){
	if(argc<2 || (argc==2 && argv[1][0] == '1')){
		printf("Argument: 1 (client) server ip addr or 0 (server)\n");
		exit(1);
	}
	if(argv[1][0]=='0'){
		printf("running server now\n");
		server();
	}else if(argv[1][0]=='1'){
		printf("running client now\n");
		client();
	}
	return 0;
}
