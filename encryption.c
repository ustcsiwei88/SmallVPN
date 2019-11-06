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

    cSSL= SSL_new(sslctx);
    SSL_set_fd(cSSL, socketfd);
    //SSL Accept portion. All reads and writes must use SSL
    int ssl_err = SSL_accept(cSSL);
    if(ssl_err <= 0)
    {
        ShutdownSSL();
    }
}

void ShowCerts(SSL* ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}

void server()
{
    printf("111");
    InitializeSSL();
    printf("222");
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
    ShowCerts(cSSL);    
    SSL_write(cSSL, "Hello", strlen("Hello"));
    printf("send msg sucessfully.\n");    
    ShutdownSSL();
}

void client()
{
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    
    SSL_load_error_strings();
    // init CTX
    sslctx = SSL_CTX_new( SSLv23_server_method());
    // open connection
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd< 0)
    {
		printf("Socket failed\n");
		exit(1);
    }
    struct sockaddr_in sa;
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
    ShowCerts(cSSL);
    int res = SSL_read(cSSL, wrbuffer, sizeof(wrbuffer));
    wrbuffer[res] = 0;
    printf("Got %d chars :%s\n", res, wrbuffer);
    SSL_CTX_free(sslctx);    
    ShutdownSSL();   
}

int main(int argc, char* argv[]){
	if(argv[1][0]=='0'){
		printf("running server now\n");
		printf("000");
		server();
	}else if(argv[1][0]=='1'){
		printf("russsnning client now\n");
		client();
	}
	return 0;
}
