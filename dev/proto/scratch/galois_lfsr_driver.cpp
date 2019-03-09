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

    int sock = 0, valread;
    struct sockaddr_un addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&addr, '0', sizeof(addr));

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, argv[1]);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    std::string pid = std::to_string(getpid());
    write(sock, pid.c_str(), pid.size());
    printf("PID sent\n");


    valread = read(sock, buffer, 1024);
    buffer[valread] = '\0';
    printf("CLIENT AYYYYOOOOO %s\n", buffer);
    unlink(argv[1]);

    return 0;

}
