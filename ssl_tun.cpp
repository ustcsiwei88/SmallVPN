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

#include <iostream>    
#include <cassert>  
#include <cstring>  
#include <vector>       
#include <openssl/rsa.h>    
#include <openssl/pem.h> 

//#include<bits/stdc++.h>
//using namespace std;
 
#define MAX_BUFF 2048

char rdbuff[MAX_BUFF];
char wrbuff[MAX_BUFF];
char encrypted_buff[MAX_BUFF];
char decrypted_buff[MAX_BUFF];

std::string Key[2];

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
    
void generateRSAKey(std::string strKey[2])  
{        
    strKey[0] = "-----BEGIN RSA PUBLIC KEY-----\nMIIBCAKCAQEAv9AV95EP5gIDDfaiyXfMwPDOrGtwVHkvuT1GuAZKnbNWa0kdNZeN\n60ZBFBALOwVVRKmnAb6vsZJiOpA8FlAVNGTZSrQunw9WVaVGzQhk2UvxMqocVJ8D\ny8JBJii5lgzccXIlyvakkhw4eHAEzOkNYFEOMx+jCniP8ZYsbtjvbBak2pzVfh5H\nC6JM0yWvh6fWF5WACgYcO1iW39h9EZsps2lewhsP/AonYuUAIX/Jkf5DJ7A2z2YF\n5zyOlJnqS6fB81rN6rUUM5ENq5y2scAYJmgi90GrOtEuRM0q3UGuf7V/ZB5egnIP\nrLE4a0dYlk1OmBstoaXcEZBhbmI/ANj3xwIBAw==\n-----END RSA PUBLIC KEY-----";  
    strKey[1] = "-----BEGIN RSA PRIVATE KEY-----\nMIIEogIBAAKCAQEAv9AV95EP5gIDDfaiyXfMwPDOrGtwVHkvuT1GuAZKnbNWa0kd\nNZeN60ZBFBALOwVVRKmnAb6vsZJiOpA8FlAVNGTZSrQunw9WVaVGzQhk2UvxMqoc\nVJ8Dy8JBJii5lgzccXIlyvakkhw4eHAEzOkNYFEOMx+jCniP8ZYsbtjvbBak2pzV\nfh5HC6JM0yWvh6fWF5WACgYcO1iW39h9EZsps2lewhsP/AonYuUAIX/Jkf5DJ7A2\nz2YF5zyOlJnqS6fB81rN6rUUM5ENq5y2scAYJmgi90GrOtEuRM0q3UGuf7V/ZB5e\ngnIPrLE4a0dYlk1OmBstoaXcEZBhbmI/ANj3xwIBAwKCAQB/4A6lC1/urAIJTxcw\n+oiAoInIR6A4UMp7fi8lWYcTzORHhhN5D7PyLtYNYAd8rjjYcRoBKcp2YZbRtX1k\nNWN4QzuHIsm/X47jw4SIsEM7h/Yhxr2Nv1fdLCtuxdEOsz2g9sPcpG22vXr69ViI\nm15ANgl3amyxpbVLuXL0kJ+dY0Y/Q/zKaOTM7AhxUJcioDp6Y7/zOWIlKIQCY0Gq\nLJJkkCMY4JK6ksWx/pPQvwJBmwbnN0i+OsxEqXh0aTCEaTFzAeX5gw/DxXsE1lsc\nJAZRObC1rGmlk7xW9jLmzxzbBY+zMgUix7wCVS+2FgiAUh4gwT0qzRrucmcqq0PT\nccPrAoGBAOXR/SKZ8hgBAUFcuB/iREH2ZzcE+Kru2wmRhJDuDmTEi1HLUQ5SvBuB\nUrXKlDPvHUDHRXnbUsMfJA5mjxbuIwAZl22+EfnJyreYqBfWEMR1wV3vy7r/RHCK\nePILJB4CJJKKSTxM9jqPCmnTRRBY5Wc5OKTgT68ye0dUjyVwORsXAoGBANWpube0\njtfXPv7MdKzxczxpmKkYN14UwMdKvwmkSelYBdhReSWRceNWlGydzRJAeHgFl8/W\nuxBhGkt/bQqW5vetQIc2XoLEHh2LsxYxeUo20Dv281HDLyM34ux35uU1CJVNChqB\nUDsaJwcG4Tt87LotwK0BIoR5aX9Z0jPTdTbRAoGBAJk2qMG79rqrViuTJWqW2Cv5\nmiSt+xyfPLELrbX0CZiDB4vc4LQ3KBJWNyPcYs1KE4CE2PvnjIIUwrREX2SewgAR\nD55+tqaGhyUQcA/kCy2j1j6f3SdU2EsG+0wHbWlWwwxcMNLd+XxfXEaM2LWQmO97\nexiVinTMUi+NtMOgJhIPAoGBAI5xJnp4XzqPf1SITcig932buxtlej64gITcf1vC\n2/DlWTrg+25g9pePDZ2+iLbVpaVZD9/kfLWWEYeqSLG570/I1a95lFctaWkHzLl2\nUNwkitKkojaCH2zP7J2lRJjOBbjeBrxWNXy8GgSvQNJTSHwegHNWFwL7m6o74Xfi\nTiSLAoGAM0e2+VKN/2nNPLluFCaVYfJ/dup4N4kF9ZKdj+iXPkzMIJypEA9sUt1q\ndUEVUfZIXYH/WuslYkoUIsi7LNYIniBdWOocKqBnKOvJiW63p32K1+omDnDQeOAw\nHF0IvZqWhyQ487/ZAjz0SNHIkmrylJwTUVJ32f30HlOzL4e3QGU=\n-----END RSA PRIVATE KEY-----";  
   
}  

std::string rsa_pub_encrypt(const std::string &clearText, const std::string &pubKey)  
{  
    std::string strRet;  
    RSA *rsa = NULL;  
    BIO *keybio = BIO_new_mem_buf((unsigned char *)pubKey.c_str(), -1);   
    rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);  
  
    int len = RSA_size(rsa);  
    char *encryptedText = (char *)malloc(len + 1);  
    memset(encryptedText, 0, len + 1);  
  
    // 加密函数  
    int ret = RSA_public_encrypt(clearText.length(), (const unsigned char*)clearText.c_str(), (unsigned char*)encryptedText, rsa, RSA_PKCS1_PADDING);  
    if (ret >= 0)  
        strRet = std::string(encryptedText, ret);  
  
    // 释放内存  
    free(encryptedText);  
    BIO_free_all(keybio);  
    RSA_free(rsa);  
  
    return strRet;  
}  
  
std::string rsa_pri_decrypt(const std::string &cipherText, const std::string &priKey)  
{  
    std::string strRet;  
    RSA *rsa = RSA_new();  
    BIO *keybio;  
    keybio = BIO_new_mem_buf((unsigned char *)priKey.c_str(), -1);  
  
    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);  
  
    int len = RSA_size(rsa);  
    char *decryptedText = (char *)malloc(len + 1);  
    memset(decryptedText, 0, len + 1);  
  
    // 解密函数  
    int ret = RSA_private_decrypt(cipherText.length(), (const unsigned char*)cipherText.c_str(), (unsigned char*)decryptedText, rsa, RSA_PKCS1_PADDING);  
    if (ret >= 0)  
        strRet = std::string(decryptedText, ret);  
  
    // 释放内存  
    free(decryptedText);  
    BIO_free_all(keybio);  
    RSA_free(rsa);  
  
    return strRet;  
}   

void* writer(void* argument){
	// int pos=0;
	int sz;
	while(1){
		sz = read(tunfd, wrbuff/*+pos*/, MAX_BUFF /*-pos*/);
		if(sz==0){
			printf("writer terminated\n");
			break;
		}
		// printf("writer sz %d\n", sz);
        std::string src = std::string(wrbuff, sz);
        std::string encrypted_str;
        encrypted_str = rsa_pub_encrypt(src, Key[0]);
        for (int i = 0; i < encrypted_str.length(); i++){
            encrypted_buff[i] = encrypted_str[i]; 
        }
		sz = encrypted_str.size();
		write(connfd, &sz, sizeof(int));
		unsigned int sz2 = write(connfd, encrypted_buff/* + pos*/, sz);
		// pos+=sz;
		// printf("read %d from tun\n", sz);
		// printf("write %d to socket\n", sz2);
		// if(pos==2048) pos=0;
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
	sa.sin_addr.s_addr = INADDR_ANY;
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
	printf("Connection established, fd: %d\n", connfd);
	pthread_t th;
	pthread_create(&th, NULL, writer, NULL);
	int pos=0;
	int sz, sz2;
	while(1){
		// printf("Wanna receive stuffs\n");
		if(read(connfd, &sz, sizeof(int))==0){
			printf("Connection terminated\n");
			close(connfd);
			close(sockfd);
			break;
		}
		// printf("sz = %d\n", sz);
		pos=0;
		while(pos<sz){
			pos+=read(connfd, rdbuff + pos, sz-pos /*- pos*/);
		}
		// printf("receive sized %d\n", sz2);
        std::string src = std::string(rdbuff,sz);
        std::string decrypted_str;
        decrypted_str = rsa_pri_decrypt(src, Key[1]);
        for (int i = 0; i < decrypted_str.length(); i++){
            decrypted_buff[i] = decrypted_str[i]; 
        }
		pos=0;
		sz = decrypted_str.size();
		while(pos<sz && pos>=0){
			pos+=write(tunfd, decrypted_buff + pos, sz-pos);
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
	pthread_t th;
	pthread_create(&th, NULL, writer, NULL);
	int pos=0;
	int sz;
	while(1){
		// printf("Wanna receive stuffs\n");
		if(read(connfd, &sz, sizeof(int))==0){
			printf("connection terminated\n");
			close(connfd);
			close(sockfd);
			break;
		}
		pos=0;
		// printf("sz = %d\n", sz);
		while(pos<sz){
			// printf("reading %d\n", pos);
			pos+=read(connfd, rdbuff+pos, sz-pos);
		}
        std::string src = std::string(rdbuff, sz);
        std::string decrypted_str;
        decrypted_str = rsa_pri_decrypt(src, Key[1]);
        for (int i = 0; i < decrypted_str.length(); i++){
            decrypted_buff[i] = decrypted_str[i]; 
        }
		pos=0;
		sz = decrypted_str.size();
		while(pos<sz && pos>=0){
			// printf("HERE %d %d\n",sz, pos);
			pos+=write(tunfd, decrypted_buff+pos, sz-pos);
		}
		// printf("client write finished\n");
		// printf("received sized %d\n", sz);
		// pos+=sz;
		// if(pos==2048) pos=0;
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
    
    generateRSAKey(Key);
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
