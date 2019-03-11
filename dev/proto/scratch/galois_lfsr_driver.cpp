#include "../modules/galois_lfsr.hpp"

#include "../sigutils1.hpp"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

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

    int m_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    SignalReceiver sh_in(m_socket, argv[1]);

    sh_in.set("pid", getpid(), SC_UINT_T);
    sh_in.send();
    // write(sock, pid.c_str(), pid.size());
    // printf("PID sent\n");


    // valread = read(sock, buffer, 1024);
    // buffer[valread] = '\0';
    // printf("CLIENT AYYYYOOOOO %s\n", buffer);
    unlink(argv[1]);

    return 0;

}
