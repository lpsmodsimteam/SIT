/**
Simple 4-bit Up-Counter Model with one clock
*/

#include "sst_counter.h"

#include <sst/core/sst_config.h>
#include <arpa/inet.h> // inet_ntoa

#define PROC_STR "proc"
#define FD_STR "fd"
#define PORT_STR "port"
#define PID_STR "pid"

// Component Constructor
sst_counter::sst_counter(SST::ComponentId_t id, SST::Params &params) : SST::Component(id) {

    // Initialize output
    m_output.init("\033[32mmodule-" + getName() + " -> \033[0m", 1, 0, SST::Output::STDOUT);

    m_port = params.find<uint16_t>("port", 2000);
    m_num_procs = params.find<int>("num_procs", 1);
    m_sysc_counter = params.find<std::string>("sysc_counter", "");
    m_sysc_inverter = params.find<std::string>("sysc_inverter", "");

    // Just register a plain clock for this simple example
    registerClock("500MHz", new SST::Clock::Handler<sst_counter>(this, &sst_counter::tick));

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

sst_counter::~sst_counter() {

    m_output.verbose(CALL_INFO, 1, 0, "Closing master socket %d...\n", m_master_sock);
    shutdown(m_master_sock, SHUT_RDWR);
    close(m_master_sock);

    for (int i = 0; i < m_num_procs; i++) {
        int fd = m_procs[i][FD_STR].get<int>();
        m_output.verbose(CALL_INFO, 1, 0, "Closing child socket %d...\n", fd);
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
}

int sst_counter::init_socks() {

    // add new socket to array of sockets
    for (int i = 0; i < m_num_procs; i++) {

        m_procs[i] = {
                {PROC_STR, m_proc_names[i]},
                {FD_STR,   0},
                {PORT_STR, 0},
                {PID_STR,  0}
        };

    }

    // create a master socket
    if (!(m_master_sock = socket(AF_INET, SOCK_STREAM, 0))) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set master socket to allow multiple connections ,
    // this is just a good habit, it will work without this
    const int opt = 1;
    if (setsockopt(m_master_sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
                   sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // type of socket created
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_ANY;
    m_addr.sin_port = htons(m_port);

    // bind the socket to localhost port 8888
    if (bind(m_master_sock, (struct sockaddr *) &m_addr, sizeof(m_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d\n", m_port);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(m_master_sock, m_num_procs * 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // accept the incoming connection
    std::cout << "Waiting for connections..." << std::endl;

    return EXIT_SUCCESS;

}

int sst_counter::sync_procs() {

    int addrlen = sizeof(m_addr);
    int sd;
    int valread;
    int connected_procs = 0;

    while (connected_procs < m_num_procs) {

        //clear the socket set
        FD_ZERO(&m_read_fds);

        //add master socket to set
        FD_SET(m_master_sock, &m_read_fds);
        int max_sd = m_master_sock;

        //add child sockets to set
        for (int i = 0; i < m_num_procs; i++) {
            //socket descriptor
            sd = m_procs[i][FD_STR];

            //if valid socket descriptor then add to read list
            if (sd > 0) {
                FD_SET(sd, &m_read_fds);
            }

            //highest file descriptor number, need it for the select function
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // wait for an activity on one of the sockets , timeout is nullptr ,
        // so wait indefinitely
        if ((select(max_sd + 1, &m_read_fds, nullptr, nullptr, nullptr) < 0)
            && (errno != EINTR)) {
            printf("select error");
        }

        // If something happened on the master socket, then its an incoming connection
        if (FD_ISSET(m_master_sock, &m_read_fds)) {

            if ((m_new_sock = accept(m_master_sock, (struct sockaddr *) &m_addr,
                                     (socklen_t *) &addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            std::string ip = inet_ntoa(m_addr.sin_addr);
            int port = ntohs(m_addr.sin_port);
            std::cout << "New connection: fd=" << m_new_sock << ", port=" << ip.c_str() << ":" << port << std::endl;
            getpeername(m_new_sock, (struct sockaddr *) &m_addr, (socklen_t *) &addrlen);

            if (!(valread = read(m_new_sock, m_buffer, BUFSIZE))) {

                //Somebody disconnected, get his details and print
                std::cout << ip.c_str() << ":" << port << " disconnected" << std::endl;

                // Close the socket and mark as 0 in list for reuse
                shutdown(m_new_sock, SHUT_RDWR);
                close(m_new_sock);
                m_procs[connected_procs][FD_STR] = 0;

            } else {

                m_buffer[valread] = '\0';

                m_procs[connected_procs][FD_STR] = m_new_sock;
                m_procs[connected_procs][PORT_STR] = port;
                m_procs[connected_procs][PID_STR] = std::stoi(m_buffer);
                connected_procs++;

            }


        }


    }

    std::cout << m_procs << std::endl;

    return EXIT_SUCCESS;

}

// setup is called by SST after a component has been constructed and should be used
// for initialization of variables
void sst_counter::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    init_socks();

    std::cout << "Master pid: " << getpid() << std::endl;

    for (int i = 0; i < m_num_procs; i++) {

        if (!fork()) {

            char *proc;

            if (i == 1) {

                proc = &m_sysc_inverter[0u];

            } else {

                proc = &m_sysc_counter[0u];

            }

            char *args[] = {proc, &(to_string(m_port))[0u], nullptr};
            m_output.verbose(CALL_INFO, 1, 0,
                             "Forking process %s (pid: %d) as \"%s\"...\n", args[0], getpid(),
                             m_proc_names[i].c_str());
            execvp(args[0], args);

        }
    }

    sync_procs();

}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void sst_counter::finish() {
    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool sst_counter::tick(SST::Cycle_t current_cycle) {

    int counter_out = 0;

    for (int proc = 0; proc < m_num_procs; proc++) {

        if (!m_procs[proc][PROC_STR].get<std::string>().compare("inverter")) {

            m_data_out["clock"] = current_cycle;
            m_data_out["on"] = true;
            m_data_out["data_in"] = counter_out;

            // turn module off at 52 ns
            if (current_cycle >= 52) {
                if (current_cycle == 52) {
                    std::cout << "MODULE OFF" << std::endl;
                }
                m_data_out["on"] = false;
            }

            // ---------------- WRITE DATA ----------------
            send_json(m_data_out, m_procs[proc][FD_STR].get<int>());

            try {

                // if module is on, dump the JSON object buffer
                if (m_data_out["on"].get<bool>()) {

                    // ---------------- READ DATA ----------------
                    m_data_in = recv_json(m_buffer, m_procs[proc][FD_STR].get<int>());

                    m_output.verbose(CALL_INFO, 1, 0, "Inverter: %d\n",
                            std::stoi(m_data_in["data_out"].get<std::string>()));
                    m_data_in.clear();

                }

            } catch (json::type_error &e) {

                std::cout << getpid() << " MASTER JSON TYPE ERROR " << e.what() << m_data_out << std::endl;

            }

            m_data_out.clear();

        } else {

            m_data_out["on"] = true;
            m_data_out["enable"] = false;
            m_data_out["reset"] = false;

            // assign SST clock to SystemC clock
            m_data_out["clock"] = current_cycle;

            // set reset to 1 at 4 ns
            if (current_cycle >= 4) {
                if (current_cycle == 4) {
                    std::cout << "RESET ON" << std::endl;
                }
                m_data_out["reset"] = true;
            }

            // set reset to 0 at 8 ns
            if (current_cycle >= 8) {
                if (current_cycle == 8) {
                    std::cout << "RESET OFF" << std::endl;
                }
                m_data_out["reset"] = false;
            }

            // set enable to 1 at 12 ns
            if (current_cycle >= 12) {
                if (current_cycle == 12) {
                    std::cout << "ENABLE ON" << std::endl;
                }
                m_data_out["enable"] = true;
            }

            // set enable to 0 at 50 ns
            if (current_cycle >= 50) {
                if (current_cycle == 50) {
                    std::cout << "ENABLE OFF" << std::endl;
                }
                m_data_out["enable"] = false;
            }

            // turn module off at 52 ns
            if (current_cycle >= 52) {
                if (current_cycle == 52) {
                    std::cout << "MODULE OFF" << std::endl;
                }
                m_data_out["on"] = false;
            }

            // ---------------- SOCKET COMMUNICATION ----------------

            // ---------------- WRITE DATA ----------------
            send_json(m_data_out, m_procs[proc][FD_STR].get<int>());

            try {

                // if module is on, dump the JSON object buffer
                if (m_data_out["on"].get<bool>()) {

                    // ---------------- READ DATA ----------------
                    m_data_in = recv_json(m_buffer, m_procs[proc][FD_STR].get<int>());

                    counter_out = std::stoi(m_data_in["data_out"].get<std::string>());

                    m_output.verbose(CALL_INFO, 1, 0, "Counter: %d\n", counter_out);
                    m_data_in.clear();

                }

            } catch (json::type_error &e) {

                std::cout << getpid() << " MASTER JSON TYPE ERROR " << e.what() << m_data_out << std::endl;

            }

            m_data_out.clear();


        }

    }

    return false;

}
