#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 80 
#define PORT 8080 
  
int main(void)
{
    struct sockaddr_in sa;
    int res;
    int SocketFD;

    SocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFD == -1) {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }
  
    memset(&sa, 0, sizeof sa);
  
    sa.sin_family = AF_INET;
    sa.sin_port = htons(1100);
    res = inet_pton(AF_INET, "127.0.0.1", &sa.sin_addr);

    if (connect(SocketFD, (struct sockaddr *)&sa, sizeof sa) == -1) {
        perror("connection with the server failed");
        close(SocketFD);
        exit(0);
    }
  
    /* perform read write operations ... */
    send(SocketFD)
    shutdown(SocketFD, SHUT_RDWR);
  
    close(SocketFD);
    return EXIT_SUCCESS;
}

void send(socket_fd){
    char buff[MAX]; 
    int n; 
    for (;;) { 
        bzero(buff, sizeof(buff)); 
        printf("Enter the string : "); 
        n = 0; 
        while ((buff[n++] = getchar()) != '\n'); 
        write(socket_fd, buff, sizeof(buff)); 
        bzero(buff, sizeof(buff)); 
        read(socket_fd, buff, sizeof(buff)); 
        printf("From server : %s", buff); 
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Client exit\n"); 
            break; 
        } 
    }
}
  