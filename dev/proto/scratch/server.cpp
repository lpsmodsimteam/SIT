// Server side C/C++ program to demonstrate Socket programming
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#define PORT "ss1"

int main(int argc, char const *argv[]) {

    int server_fd, new_socket, valread;
    struct sockaddr_un address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sun_family = AF_UNIX;
    // address.sun_addr.s_addr = INADDR_ANY;
    strcpy(address.sun_path, PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    valread = read(new_socket , buffer, 1024);
    printf("%s\n", buffer );
    send(new_socket , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    unlink(PORT);
    return 0;
}