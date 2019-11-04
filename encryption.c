#include <openssl/applink.c>
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
char wrbuffer[2048]

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
    ssl_err = SSL_accept(cSSL);
    if(ssl_err <= 0)
    {
        ShutdownSSL();
    }
}

int ReadSSl(char buf, int len2read)
{
    SSL_read(cSSL, *buf, len2read);
}

void WriteSSl(char *str, int len)
{
    SSL_write(cSSL, str, len);
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
    struct sockaddr_in saiServerAddress;
    bzero((char *) &saiServerAddress, sizeof(saiServerAddress));
    saiServerAddress.sin_family = AF_INET;
    saiServerAddress.sin_addr.s_addr = serv_addr;
    saiServerAddress.sin_port = htons(8080);

    bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    listen(sockfd,5);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    UseSSL(newsockfd);
    SSL_write(cSSL, "Hello", strlen("Hello")));
    ShutdownSSL();
}

void client()
{
        InitializeSSL();
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd< 0)
    {
		printf("Socket failed\n");
		exit(1);
    }
    struct sockaddr_in saiServerAddress;
    bzero((char *) &saiServerAddress, sizeof(saiServerAddress));
    saiServerAddress.sin_family = AF_INET;
    saiServerAddress.sin_addr.s_addr = serv_addr;
    saiServerAddress.sin_port = htons(8080);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    }

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    UseSSL(newsockfd);
    res = SSL_write(cSSL, wrbuffer, sizeof(wrbuffer) - 1));
	printf("Got %d chars :%s\n", res, wrbuffer);
    ShutdownSSL();   
}