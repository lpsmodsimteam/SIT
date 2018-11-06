/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("ERROR opening socket");
    }

    struct sockaddr_in serv_addr, cli_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));

    int portno = 8080;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
    }

    listen(sockfd, 5);

    socklen_t clilen = sizeof(cli_addr);

    int newsockfd = accept(sockfd,
                       (struct sockaddr *) &cli_addr,
                       &clilen);
    if (newsockfd < 0) {
        perror("ERROR on accept");
    }

    char buffer[256];
    bzero(buffer, 256);
    int n = read(newsockfd, buffer, 255);

    if (n < 0) {
        perror("ERROR reading from socket");
    }
    printf("Here is the message: %s\n", buffer);

    n = write(newsockfd, "I got your message", 18);

    if (n < 0) {
        perror("ERROR writing to socket");
    }

    close(newsockfd);
    close(sockfd);

    return 0;

}