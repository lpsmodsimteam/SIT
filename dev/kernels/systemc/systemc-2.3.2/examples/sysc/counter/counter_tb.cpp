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


void send_signal(const json &data, int sock_fd) {

    // convert JSON object to bytes to be transmitted via sockets
    std::string data_str = to_string(data);
    if (write(sock_fd, data_str.c_str(), data_str.size()) < 0) {
        perror("ERROR writing to socket");
    }

}

json recv_signal(char buffer[], int sock_fd) {

    if (sock_fd < 0) {
        perror("ERROR on accept");
    }

    // reset buffer
    bzero(buffer, BUFSIZE);

    // read buffer from child process
    if (read(sock_fd, buffer, BUFSIZE - 1) < 0) {
        perror("ERROR reading from socket");
    }

    try {
        return json::parse(buffer);
    } catch (json::parse_error &e) {
        std::cout << "JSON PARSE ERROR" << buffer << std::endl;
        exit(-1);
    }
}


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
    uint16_t portno = 8080;
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

        m_data_in = recv_signal(m_buffer, sockfd);

        clock = (int) (m_data_in["clock"]) % 2;
        enable = (int) m_data_in["enable"];
        reset = (int) m_data_in["reset"];

        m_data_out["counter_out"] = to_string(counter_out);

        send_signal(m_data_out, sockfd);

        std::cout << "@" << sc_time_stamp() << " sst-timestamp: " << m_data_in["clock"] <<
                  " clock: " << clock << " enable: " << m_data_in["enable"]
                  << " reset: " << m_data_in["reset"] << std::endl;

    } while (m_data_in["on"]);

    close(sockfd);

    return 0;// Terminate simulation

}

int main(int argc, char *argv[]) {

    // directly invoke inner function
    sc_core::sc_elab_and_sim(argc, argv);

    // invoke wrapper - both can't be called
    // sc_main(argc, argv);

    return EXIT_SUCCESS;

}
