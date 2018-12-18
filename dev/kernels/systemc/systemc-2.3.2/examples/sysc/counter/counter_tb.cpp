#include "systemc.h"
#include "counter.cpp"

#include "json.hpp"
using json = nlohmann::json;

#include <sstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

template<typename T>
std::string to_string(const T &value) {
    std::ostringstream ss;
    ss << value;
    return ss.str();
}


int sc_main(int argc, char *argv[]) {

    sc_signal<bool> clock;
    sc_signal<bool> reset;
    sc_signal<bool> enable;
    sc_signal<sc_uint<8> > counter_out;

    // Connect the DUT
    sysc_counter sysc_counter("COUNTER");
    sysc_counter.clock(clock);
    sysc_counter.reset(reset);
    sysc_counter.enable(enable);
    sysc_counter.counter_out(counter_out);

    char m_buffer[BUFSIZE];
    int portno = 8080;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *server = gethostbyname("work-vm01");
    struct sockaddr_in serv_addr{};

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    while (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(-1);
    }

    // create an empty structure (null)
    json m_data_in;
    json m_data_out;

    do {

        sc_start(1, SC_NS);

        m_data_out["counter_out"] = to_string(counter_out);

        std::string counter_out_str = to_string(m_data_out);

        if (write(sockfd, counter_out_str.c_str(), counter_out_str.size()) < 0) {
            perror("ERROR writing to socket");
        }

        bzero(m_buffer, BUFSIZE);
        if (read(sockfd, m_buffer, BUFSIZE - 1) < 0) {
            perror("ERROR reading from socket");
        }

        m_data_in = json::parse(m_buffer);

        clock =  (int(m_data_in["clock"]) % 2);
        enable = (int) m_data_in["enable"];
        reset = (int) m_data_in["reset"];

        std::cout << "@ " << sc_time_stamp() << " clock: " << m_data_in["clock"] << ' ' << clock << std::endl;
        std::cout << "@ " << sc_time_stamp() << " enable: " << m_data_in["enable"] << std::endl;
        std::cout << "@ " << sc_time_stamp() << " reset: " << m_data_in["reset"] << std::endl;

    } while (m_data_in["on"]);

//    sc_start(1, SC_NS);
//
//    // Initialize all variables
//    reset = 0;       // initial value of reset
//    enable = 0;      // initial value of enable
//    for (int i = 0; i < 5; i++) {
//        clock = 0;
//        sc_start(1, SC_NS);
//        clock = 1;
//        sc_start(1, SC_NS);
//    }
//    reset = 1;    // Assert the reset
//    cout << "@" << sc_time_stamp() << " Asserting reset\n\n";
//    for (int i = 0; i < 10; i++) {
//        clock = 0;
//        sc_start(1, SC_NS);
//        clock = 1;
//        sc_start(1, SC_NS);
//    }
//    reset = 0;    // De-assert the reset
//    cout << "@" << sc_time_stamp() << " De-Asserting reset\n\n";
//    for (int i = 0; i < 5; i++) {
//        clock = 0;
//        sc_start(1, SC_NS);
//        clock = 1;
//        sc_start(1, SC_NS);
//    }
//    cout << "@" << sc_time_stamp() << " Asserting Enable\n\n";
//
//    if (sockfd < 0) {
//        perror("ERROR opening socket");
//    }
//
//    if (!server) {
//        fprintf(stderr, "ERROR, no such host\n");
//        exit(0);
//    }
//
//
//    enable = 1;  // Assert enable
//    for (int i = 0; i < 260; i++) {
//        clock = 0;
//        sc_start(1, SC_NS);
//        clock = 1;
//        sc_start(1, SC_NS);
//
//
//    }
//    cout << "@" << sc_time_stamp() << " De-Asserting Enable\n\n";
//    enable = 0; // De-assert enable
//
//    cout << "@" << sc_time_stamp() << " Terminating simulation\n\n";

    return 0;// Terminate simulation

}

int main(int argc, char *argv[]) {

    // directly invoke inner function
    sc_core::sc_elab_and_sim(argc, argv);

    // invoke wrapper - both can't be called
    // sc_main(argc, argv);

    return EXIT_SUCCESS;

}
