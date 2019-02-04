/**
Simple 4-bit Up-Counter Model with one clock
*/

#include "sst_counter.h"

#include <sst/core/sst_config.h>
#include <arpa/inet.h> // inet_ntoa


// Component Constructor
sst_counter::sst_counter(SST::ComponentId_t id, SST::Params &params) : SST::Component(id) {

    // Initialize output
    m_output.init("module-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

    m_port = params.find<uint16_t>("port", 2000);
    m_sysc_counter = params.find<std::string>("sysc_counter", "");

    // Just register a plain clock for this simple example
    registerClock("500MHz", new SST::Clock::Handler<sst_counter>(this, &sst_counter::tick));

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

sst_counter::~sst_counter() {
    close(m_newsockfd);
    close(m_sockfd);
}

// setup is called by SST after a component has been constructed and should be used
// for initialization of variables
void sst_counter::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");


    ///////////////////////////////////////////////

    struct sockaddr_in parent_addr;

    //set of socket descriptors
    fd_set read_fds;

    //a message
    std::string message("ECHO Daemon v1.0 \r\n");

    int max_clients = 30;
    int client_socket[max_clients];
    // initialize all client_socket[] to 0 so not checked
    for (int i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    int master_sock;
    //create a master socket
    if (!(master_sock = socket(AF_INET, SOCK_STREAM, 0))) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    int opt = 1;
    if (setsockopt(master_sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt,
                   sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // type of socket created
    parent_addr.sin_family = AF_INET;
    parent_addr.sin_addr.s_addr = INADDR_ANY;
    parent_addr.sin_port = htons(m_port);

    // bind the socket to localhost port 8888
    if (bind(master_sock, (struct sockaddr *) &parent_addr, sizeof(parent_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d\n", m_port);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_sock, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    int addrlen = sizeof(parent_addr);
    puts("Waiting for connections ...");

    ///////////////////////////////////////////////


    done = 0;

    int procs = 3;

    for (int i = 0; i < procs; i++) {

        pids.push_back(fork());

        if (!pids.back()) {

            char *args[] = {&m_sysc_counter[0u], &(to_string(m_port))[0u], nullptr};
            m_output.verbose(CALL_INFO, 1, 0,
                             "Forking process %s (pid: %d)\n", args[0], getpid());
            execvp(args[0], args);

//        } else {

//            m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
//
//            if (m_sockfd < 0) {
//                perror("PARENT");
//            }
//
//            bzero((char *) &serv_addr, sizeof(serv_addr));
//
//            serv_addr.sin_family = AF_INET;
//            serv_addr.sin_addr.s_addr = INADDR_ANY;
//            serv_addr.sin_port = htons(m_port + i);
//
//            if (bind(m_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
//                perror("PARENT");
//                exit(-1);
//            }
//
//            listen(m_sockfd, SOMAXCONN);
//
//            m_clilen = sizeof(cli_addr);
//
//            m_newsockfd = accept(m_sockfd, (struct sockaddr *) &cli_addr, &m_clilen);
//
//            if (!done) {
//
//                m_data_out["on"] = true;
//                m_data_out["clock"] = 0;
//                m_data_out["reset"] = 0;
//                m_data_out["enable"] = 0;
//                std::cout << "CHILD 1" << std::endl;
//                done = 1;
//
//            }

        }
    }


    //////////////////////////////////////////////

    int sd, max_sd;
    int valread;
    int new_socket;
    int children = 0;

    while (children < procs) {

        //clear the socket set
        FD_ZERO(&read_fds);

        //add master socket to set
        FD_SET(master_sock, &read_fds);
        max_sd = master_sock;

        //add child sockets to set
        for (int i = 0; i < max_clients; i++) {
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if (sd > 0) {
                FD_SET(sd, &read_fds);
            }

            //highest file descriptor number, need it for the select function
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // wait for an activity on one of the sockets , timeout is nullptr ,
        // so wait indefinitely
        if ((select(max_sd + 1, &read_fds, nullptr, nullptr, nullptr) < 0)
            && (errno != EINTR)) {
            printf("select error");
        }

        // If something happened on the master socket, then its an incoming connection
        if (FD_ISSET(master_sock, &read_fds)) {

            if ((new_socket = accept(master_sock, (struct sockaddr *) &parent_addr,
                                     (socklen_t * ) & addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n",
                   new_socket, inet_ntoa(parent_addr.sin_addr), ntohs(parent_addr.sin_port));

            //send new connection greeting message
            if (write(new_socket, message.c_str(), message.length()) != message.length()) {
                perror("send");
            }

            puts("Welcome message sent successfully");

            //add new socket to array of sockets
            for (int i = 0; i < max_clients; i++) {
                //if position is empty
                if (!client_socket[i]) {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    children++;

                    break;
                }
            }

        }


    }


    //////////////////////////////////////////////

}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void sst_counter::finish() {
    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool sst_counter::tick(SST::Cycle_t current_cycle) {


    ////////////////////////////////////////////


    //        //else its some IO operation on some other socket
//        for (int i = 0; i < max_clients; i++) {
//            sd = client_socket[i];
//
//            if (FD_ISSET(sd, &read_fds)) {
//                //Check if it was for closing , and also read the
//                //incoming message
//                if (!(valread = read(sd, m_buffer, 1024))) {
//
//                    //Somebody disconnected, get his details and print
//                    getpeername(sd, (struct sockaddr *) &parent_addr, (socklen_t * ) & addrlen);
//                    printf("Client disconnected, ip %s , port %d \n",
//                           inet_ntoa(parent_addr.sin_addr), ntohs(parent_addr.sin_port));
//
//                    //Close the socket and mark as 0 in list for reuse
//                    close(sd);
//                    client_socket[i] = 0;
//
//                } else {  // Echo back the message that came in
//
//                    getpeername(sd, (struct sockaddr *) &parent_addr, (socklen_t * ) & addrlen);
//                    printf("YO ip %s , port %d \n",
//                           inet_ntoa(parent_addr.sin_addr), ntohs(parent_addr.sin_port));
//                    printf("BUF: %s", m_buffer);
//
//                    m_buffer[valread] = '\0';
//                    write(sd, m_buffer, valread);
//
//                }
//
//            }
//        }


    /////////////////////////////////////////////////


    if (done) {

        // ---------------- SYSTEMC MODULE TESTBENCH ----------------

        // assign SST clock to SystemC clock
        m_data_out["clock"] = current_cycle;

        // set reset to 1 at 4 ns
        if (current_cycle == 4) {
            std::cout << "RESET ON" << std::endl;
            m_data_out["reset"] = 1;
        }

        // set reset to 0 at 8 ns
        if (current_cycle == 8) {
            std::cout << "RESET OFF" << std::endl;
            m_data_out["reset"] = 0;
        }

        // set enable to 1 at 12 ns
        if (current_cycle == 12) {
            std::cout << "ENABLE ON" << std::endl;
            m_data_out["enable"] = 1;
        }

        // set enable to 0 at 50 ns
        if (current_cycle == 50) {
            std::cout << "ENABLE OFF" << std::endl;
            m_data_out["enable"] = 0;
        }

        // turn module off at 52 ns
        if (current_cycle == 52) {
            std::cout << "MODULE OFF" << std::endl;
            m_data_out["on"] = false;
        }

        // ---------------- SOCKET COMMUNICATION ----------------

        // ---------------- WRITE DATA ----------------
        send_signal(m_data_out, m_newsockfd);

        // if module is on, dump the JSON object buffer
        if (m_data_out["on"]) {

            // ---------------- READ DATA ----------------
            m_data_in = recv_signal(m_buffer, m_newsockfd);

            m_output.verbose(CALL_INFO, 1, 0, "Counter: %s \n",
                             std::string(m_data_in["counter_out"]).c_str());

        }
    }

    return false;

}
