#include "sysc_counter.cpp"

#include "sstsysc.hpp"
#include <systemc.h>


int sc_main(int argc, char *argv[]) {

    sc_signal<bool> clock;
    sc_signal<bool> reset;
    sc_signal<bool> enable;
    sc_signal<sc_uint<4> > counter_out;

    // Connect the DUT
    sysc_counter sysc_counter("COUNTER");
    sysc_counter.clock(clock);
    sysc_counter.reset(reset);
    sysc_counter.enable(enable);
    sysc_counter.counter_out(counter_out);

    char m_buffer[BUFSIZE];
    uint16_t portno = (uint16_t) std::stoi(argv[1]);
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *server = gethostbyname("work-vm01");
    struct sockaddr_in serv_addr{};

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("CHILD");
        exit(-1);
    }

    // create an empty structure (null)
    json m_data_in;
    json m_data_out;

    std::cout << "CHILD PID: " << getpid() << '\n';

//    do {

        sc_start(1, SC_NS);

        if (sock_fd < 0) {
            perror("ERROR on accept");
        }

        // reset buffer
        bzero(m_buffer, BUFSIZE);

        int valread;

        // read buffer from child process
        if ((valread = read(sock_fd, m_buffer, BUFSIZE - 1)) < 0) {
            perror("ERROR reading from socket");
        }

        std::cout << getpid() << ' ' << m_buffer;

        write(sock_fd, "HOHO Daemon v1.0 \r\n", valread);
//        m_data_in = recv_signal(m_buffer, sock_fd);
//
//
//        clock = (int) (m_data_in["clock"]) % 2;
//        enable = (int) m_data_in["enable"];
//        reset = (int) m_data_in["reset"];
//
//        m_data_out["counter_out"] = to_string(counter_out);
//
//        send_signal(m_data_out, sock_fd);
//
//        std::cout << "@" << sc_time_stamp() << " sst-timestamp: " << m_data_in["clock"] <<
//                  " clock: " << clock << " enable: " << m_data_in["enable"]
//                  << " reset: " << m_data_in["reset"] << std::endl;

//    } while (m_data_in["on"]);

    close(sock_fd);

    return 0;  // Terminate simulation

}
