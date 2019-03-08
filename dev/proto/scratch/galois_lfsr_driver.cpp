#include "../modules/galois_lfsr.hpp"

#include "../sstscit.hpp"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define CLIENT_SOCK_FILE "client.sock"
#define SERVER_SOCK_FILE "server0.sock"

int sc_main(int argc, char *argv[]) {

    // ---------- SYSTEMC UUT INIT ---------- //
    sc_signal<bool> clock;
    sc_signal<bool> reset;
    sc_signal<sc_uint<4> > data_out;

    // Connect the DUT
    galois_lfsr DUT("GALOIS_LFSR");
    DUT.clock(clock);
    DUT.reset(reset);
    DUT.data_out(data_out);
    // ---------- SYSTEMC UUT INIT ---------- //
    int fd;
    struct sockaddr_un addr{};
    int ret;
    char buff[8192];
    struct sockaddr_un from{};
    int ok = 1;
    ssize_t len;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        ok = 0;
    }

    if (ok) {
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, CLIENT_SOCK_FILE);
        unlink(CLIENT_SOCK_FILE);
        if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
            perror("bind");
            ok = 0;
        }
    }

    if (ok) {
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, SERVER_SOCK_FILE);
        if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
            perror("connect");
            ok = 0;
        }
    }

    if (ok) {
        strcpy(buff, "iccExchangeAPDU");
        if (write(fd, buff, strlen(buff) + 1) == -1) {
            perror("send");
            ok = 0;
        }
        printf("sent iccExchangeAPDU\n");
    }

    if (ok) {
        if ((len = read(fd, buff, 8192)) < 0) {
            perror("recv");
            ok = 0;
        }
        printf("receive %zd %s\n", len, buff);
    }

    if (fd >= 0) {
        close(fd);
    }

    unlink(CLIENT_SOCK_FILE);
    return 0;

}
