//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux 
#include <arpa/inet.h> // inet_ntoa
#include <stdlib.h>
#include <string>
#include <sys/socket.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <unistd.h> // read and close
#include <mpi.h>
#define PORT 8888

int main(int argc, char *argv[]) {

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    printf("Hello world from processor %s, rank %d out of %d processors\n",
           processor_name, world_rank, world_size);

    // Finalize the MPI environment.
    MPI_Finalize();

    struct sockaddr_in parent_addr;

    char m_buffer[1025]; //data m_buffer of 1K

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
    parent_addr.sin_port = htons(PORT);

    // bind the socket to localhost port 8888
    if (bind(master_sock, (struct sockaddr *) &parent_addr, sizeof(parent_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d\n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_sock, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    int addrlen = sizeof(parent_addr);
    puts("Waiting for connections ...");

    int sd, max_sd;
    int valread;
    int new_socket;

    while (true) {
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

                    break;
                }
            }

        }

        //else its some IO operation on some other socket
        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &read_fds)) {
                //Check if it was for closing , and also read the
                //incoming message
                if (!(valread = read(sd, m_buffer, 1024))) {

                    //Somebody disconnected, get his details and print
                    getpeername(sd, (struct sockaddr *) &parent_addr, (socklen_t * ) & addrlen);
                    printf("Client disconnected, ip %s , port %d \n",
                           inet_ntoa(parent_addr.sin_addr), ntohs(parent_addr.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket[i] = 0;

                } else {  // Echo back the message that came in

                    getpeername(sd, (struct sockaddr *) &parent_addr, (socklen_t * ) & addrlen);
                    printf("YO ip %s , port %d \n",
                           inet_ntoa(parent_addr.sin_addr), ntohs(parent_addr.sin_port));
                    printf("BUF: %s\n", m_buffer);

                    m_buffer[valread] = '\0';
                    write(sd, m_buffer, valread);

                }

            }
        }

    }

    return 0;
} 
