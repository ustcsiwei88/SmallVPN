#include "encryption.h"

void socket_server()
{
    char str[INET_ADDRSTRLEN];
    int port = 10010;

    int servfd = socket(AF_INET, SOCK_STREAM, 0);
    if (servfd < 0)
        die("socket()");

    int enable = 1;
    if (setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
        die("setsockopt(SO_REUSEADDR)");

    /* Specify socket address */
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(servfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        die("bind()");

    if (listen(servfd, 10) < 0)
        die("listen()");

    int clientfd;
    struct sockaddr_in peeraddr;
    socklen_t peeraddr_len = sizeof(peeraddr);

    struct pollfd fdset[2];
    memset(&fdset, 0, sizeof(fdset));

    // int new_fd = open("foobar.txt", O_RDWR);

    fdset[0].fd = STDIN_FILENO;
    // fdset[0].fd = new_fd;
    fdset[0].events = POLLIN;

    ssl_init("server.crt", "server.key");

    while (1) {
        printf("waiting for next connection on port %d\n", port);

        clientfd = accept(servfd, (struct sockaddr *)&peeraddr, &peeraddr_len);
        if (clientfd < 0)
            die("accept()");

        ssl_client_init(&client, clientfd, SSLMODE_SERVER);

        inet_ntop(peeraddr.sin_family, &peeraddr.sin_addr, str, INET_ADDRSTRLEN);
        printf("new connection from %s:%d\n", str, ntohs(peeraddr.sin_port));

        fdset[1].fd = clientfd;

        /* event loop */

        fdset[1].events = POLLERR | POLLHUP | POLLNVAL | POLLIN;
#ifdef POLLRDHUP
        fdset[1].events |= POLLRDHUP;
#endif

        while (1) {
            fdset[1].events &= ~POLLOUT;
            fdset[1].events |= (ssl_client_want_write(&client)? POLLOUT : 0);

            // write(new_fd, "hello client\n", strlen("hello client\n")); 

            int nready = poll(&fdset[0], 2, -1);

            if (nready == 0)
                continue; /* no fd ready */

            int revents = fdset[1].revents;
            if (revents & POLLIN)
                if (do_sock_read() == -1)
                break;
            if (revents & POLLOUT)
                if (do_sock_write() == -1)
                break;
            if (revents & (POLLERR | POLLHUP | POLLNVAL))
                break;
#ifdef POLLRDHUP
            if (revents & POLLRDHUP)
                break;
#endif
            if (fdset[0].revents & POLLIN)
                // do_read(new_fd);
                do_stdin_read();

            if (client.encrypt_len>0)
                do_encrypt();
        }

        close(fdset[1].fd);
        ssl_client_cleanup(&client);
    }   
}

void socket_client()
{
    int port = 10010;
    char* host="165.227.181.114";

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        die("socket()");

    /* Specify socket address */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &(addr.sin_addr)) <= 0)
        die("inet_pton()");

    if (connect(sockfd, (struct sockaddr*) &addr, sizeof(addr)) < 0)
        die("connect()");

    struct pollfd fdset[2];
    memset(&fdset, 0, sizeof(fdset));



    fdset[0].fd = STDIN_FILENO;
    // int new_fd = open("foobar.txt", O_RDWR);
    // fdset[0].fd = new_fd;
    fdset[0].events = POLLIN;

    // ssl_init("/serverCertificate.pem","/privkey.pem");
    ssl_init(0,0);
    ssl_client_init(&client, sockfd, SSLMODE_CLIENT);

    fdset[1].fd = sockfd;
    fdset[1].events = POLLERR | POLLHUP | POLLNVAL | POLLIN;
#ifdef POLLRDHUP
    fdset[1].events |= POLLRDHUP;
#endif

    /* event loop */

    do_ssl_handshake();

    while (1) {
        fdset[1].events &= ~POLLOUT;
        fdset[1].events |= ssl_client_want_write(&client)? POLLOUT:0;
        // write(new_fd, "hello server\n", strlen("hello server\n")); 
        int nready = poll(&fdset[0], 2, -1);

        if (nready == 0)
        continue; /* no fd ready */

        int revents = fdset[1].revents;
        if (revents & POLLIN)
        if (do_sock_read() == -1)
            break;
        if (revents & POLLOUT)
        if (do_sock_write() == -1)
            break;
        if (revents & (POLLERR | POLLHUP | POLLNVAL))
        break;
#ifdef POLLRDHUP
        if (revents & POLLRDHUP)
        break;
#endif
        if (fdset[0].revents & POLLIN)
        do_stdin_read();
        // do_read(new_fd);
        if (client.encrypt_len>0)
        do_encrypt();
    }

    close(fdset[1].fd);
    ssl_client_cleanup(&client); 
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("server 0, client 1\n");
        exit(1);
    }   
	if(argv[1][0]=='0'){
		printf("running server now\n");
		socket_server();
	}else if(argv[1][0]=='1'){
		printf("running client now\n");
		socket_client();
	}
	return 0;
}
