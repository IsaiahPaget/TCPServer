#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#define PORT 5555
#define BACKLOG 10
/**
* Encapsulates the properties of  the server
*/
typedef struct Server {
    // file descriptor of the socket in passive
    // mode to wait for connections
    int listen_fd;
} server_t;

// creates a socket for the servers and makes it passive so we can wait for connections on it later.
int server_listen(server_t* server) {
    int err = 0;
    // get socket and store it in server
    err = (server->listen_fd = socket(AF_INET, SOCK_STREAM, 0));
    if (err == -1) {
        perror("socket");
        printf("Failed to create socket endpoint\n");
        return err;
    }

    struct sockaddr_in server_addr = { 0 };
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // bind and address to the socket
    err = bind(
        server->listen_fd,
        (struct sockaddr*)&server_addr,
        sizeof(server_addr)
    );
    if (err == -1) {
        perror("bind");
        printf("Failed to bind socket to address\n");
        return err;
    }

    // make socket passive with listen
    err = listen(
        server->listen_fd,
        BACKLOG
    );
    if (err == -1) {
        perror("listen");
        printf("Failed to put socket in passive mode\n");
        return err;
    }
}

// accepts new connections and then prints 'hello, world' to them.
int server_accept(server_t* server) {
    int err = 0;
    int conn_fd;
    socklen_t client_len;

    struct sockaddr_in client_addr;

    client_len = sizeof(client_addr);
    err = (
        conn_fd = accept(
            server->listen_fd,
            (struct sockaddr*)&client_addr,
            &client_len
        )
    );
    if (err == -1) {
        perror("accept");
        printf("Failed accepting connection\n");
        return err;
    }

    printf("Client connect!\n");
    
    err = close(conn_fd);
    if (err == -1) {
        perror("close");
        printf("Failed close connection\n");
        return err;
    }

    return err;
}

int main(int argc, char *argv[])
{
    int err = 0;
    server_t server = { 0 };

    err = server_listen(&server);
    if (err) {
        printf("Failed to listen on address 0.0.0.0:%d\n", PORT);
        return err;
    }

    for (;;) {
        err = server_accept(&server);
        if (err) {
            printf("Failed accepting connection\n");
            return err;
        }
    }

    return 0;
}
