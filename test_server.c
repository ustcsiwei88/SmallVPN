#include "encryption.h"

#define PORT 10010

int main(int argc, char const *argv[]) 
{ 
    char str[INET_ADDRSTRLEN];
    int server_fd, new_socket;

    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(1); 
    } 

    struct sockaddr_in address, peeraddr; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    char *hello = "Hello from server"; 

    // Forcefully attaching socket to the port 10010 
    int opt = 1; 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
    { 
        perror("setsockopt"); 
        exit(1); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(PORT); 
       
    // Forcefully attaching socket to the port 10010 
    if (bind(server_fd, (struct sockaddr *)&address, addrlen)<0) 
    { 
        perror("bind failed"); 
        exit(1); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(1); 
    } 

    ssl_init("server.crt", "server.key");
    if ((new_socket = accept(server_fd, (struct sockaddr *)&peeraddr, (socklen_t*)&peeraddr)) < 0) 
    { 
        printf("accept error: %s\n", strerror(errno));
		exit(1);
    } 
    ssl_client_init(&client, new_socket, SSLMODE_SERVER);
    inet_ntop(peeraddr.sin_family, &peeraddr.sin_addr, str, INET_ADDRSTRLEN);
    printf("new connection from %s:%d\n", str, ntohs(peeraddr.sin_port));

    // valread = read( new_socket , buffer, 1024); 
    // printf("%s\n",buffer ); 
    // send(new_socket , hello , strlen(hello) , 0 ); 
    if (do_sock_write() == -1){
        printf("socket write error");
		exit(1);
    }
    do_stdin_read();
    do_encrypt();
    printf("Hello message sent\n"); 
    close(new_socket);
    ssl_client_cleanup(&client);
    return 0; 
} 