#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*
 * This  program creates a  pair of connected sockets,
 * then forks and communicates over them.  This is very 
 * similar to communication with pipes, however, socketpairs
 * are  two-way  communications  objects.  Therefore I can
 * send messages in both directions.
 */

int main() {
    int sockets[2], child;
    char buf[1024];
    char data[20];
    strncpy(data, "CHILD", sizeof(data) - 1);
    data[sizeof(data) - 1] = 0;

    printf("LOLOL\n");

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
        perror("opening stream socket pair");
        exit(1);
    }

    if ((child = fork()) == -1)
        perror("fork");
    else if (child) {       /* This is the parent. */
        close(sockets[0]);
        if (read(sockets[1], buf, sizeof(buf)) < 0)
            perror("reading stream message");
        printf("-->PARENT: %s\n", buf);
        if (write(sockets[1], data, sizeof(data)) < 0)
            perror("writing stream message");
        close(sockets[1]);
    } else {                /* This is the child. */
        close(sockets[1]);
        strncpy(data, "PARENT", sizeof(data) - 1);
        data[sizeof(data) - 1] = 0;
        if (write(sockets[0], data, sizeof(data)) < 0)
            perror("writing stream message");
        if (read(sockets[0], buf, sizeof(buf)) < 0)
            perror("reading stream message");
        printf("-->CHILD: %s\n", buf);
        close(sockets[0]);
    }
    printf("OUT\n");

}
