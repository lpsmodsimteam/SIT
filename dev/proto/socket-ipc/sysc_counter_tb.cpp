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


    if (sock_fd < 0) {
        perror("ERROR on accept");
    }


    std::string pid = to_string(getpid());
    if (write(sock_fd, pid.c_str(), pid.size()) != pid.size()) {
        perror("ERROR writing to socket");
    }

//    m_data_in = recv_json(m_buffer, sock_fd);

    try {

        bool flag;

        do {

            char m_buffer[BUFSIZE];

            sc_start(1, SC_NS);

            m_data_in = recv_json(m_buffer, sock_fd);

//            std::cout << getpid() << " GOT DATA " << m_data_in << std::endl;
            flag = m_data_in["on"].get<bool>();
            clock = (m_data_in["clock"].get<int>()) % 2;
            enable = m_data_in["enable"].get<int>();
            reset = m_data_in["reset"].get<int>();

            m_data_out["counter_out"] = to_string(counter_out);

            send_json(m_data_out, sock_fd);
            m_data_out.clear();

            std::cout << getpid() << " @" << sc_time_stamp() << " sst-timestamp: " << m_data_in["clock"] <<
                      " clock: " << clock << " enable: " << m_data_in["enable"]
                      << " reset: " << m_data_in["reset"] << std::endl;
            m_data_in = json{};
            std::cout << getpid() << " @" << sc_time_stamp() << " buffer cleared: " << m_data_in << std::endl;

        } while (flag);

    } catch (json::type_error &e) {

        std::cout << RED << getpid() << DEF << " CHILD JSON TYPE ERROR " << e.what() << m_data_in << std::endl;

    }


    close(sock_fd);

    return 0;  // Terminate simulation

}
