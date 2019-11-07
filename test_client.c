#include "encryption.h"

#define PORT 10010 

int main(int argc, char const *argv[]) 
{   
    char* host="127.0.0.1";

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        die("socket()");

    /* Specify socket address */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, host, &(addr.sin_addr)) <= 0)
        die("inet_pton()");

    if (connect(sockfd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
        die("connect()");

    ssl_init(0,0);
    ssl_client_init(&client, sockfd, SSLMODE_CLIENT);

    do_ssl_handshake();

    do_sock_read();
    do_stdin_read();
    do_encrypt();

    close(STDIN_FILENO);
    ssl_client_cleanup(&client); 
} 