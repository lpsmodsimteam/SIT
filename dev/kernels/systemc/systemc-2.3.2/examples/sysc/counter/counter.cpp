//-----------------------------------------------------
// This is my second Systemc Example
// Design Name : counter
// File Name : counter.cpp
// Function : This is a 4 bit up-counter with
// Synchronous active high reset and
// with active high enable signal
//-----------------------------------------------------
#include "systemc.h"

///* A simple server in the internet domain using TCP
//   The port number is passed as an argument */
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>

//// socket stuff
//void error(const char *msg) {
//    perror(msg);
//    exit(1);
//}
//
//int conn(const std::string& msg) {
//
//    int sockfd, newsockfd, portno;
//    socklen_t clilen;
//    char buffer[256];
//    struct sockaddr_in serv_addr, cli_addr;
//    int n;
//    sockfd = socket(AF_INET, SOCK_STREAM, 0);
//    if (sockfd < 0)
//        error("ERROR opening socket");
//    bzero((char *) &serv_addr, sizeof(serv_addr));
//    portno = 8000;
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_addr.s_addr = INADDR_ANY;
//    serv_addr.sin_port = htons(portno);
//    if (bind(sockfd, (struct sockaddr *) &serv_addr,
//             sizeof(serv_addr)) < 0)
//        error("ERROR on binding");
//    listen(sockfd, 5);
//    clilen = sizeof(cli_addr);
//    newsockfd = accept(sockfd,
//                       (struct sockaddr *) &cli_addr,
//                       &clilen);
//    if (newsockfd < 0)
//        error("ERROR on accept");
//    bzero(buffer, 256);
//    n = read(newsockfd, buffer, 255);
//    if (n < 0) error("ERROR reading from socket");
//    printf("Here is the message: %s\n", buffer);
//    cout << msg << endl;
//    n = write(newsockfd, "I got your message", 18);
//    if (n < 0) error("ERROR writing to socket");
//    close(newsockfd);
//    close(sockfd);
//    return 0;
//
//}


SC_MODULE (counter) {

    sc_in_clk clock;      // Clock input of the design
    sc_in<bool> reset;      // active high, synchronous Reset input
    sc_in<bool> enable;      // Active high enable signal for counter
    sc_out< sc_uint<4> > counter_out; // 4 bit vector output of the counter

    //------------Local Variables Here---------------------
    sc_uint<4> count;

    //------------Code Starts Here-------------------------
    // Below function implements actual counter logic
    void incr_count() {
        // At every rising edge of clock we check if reset is active
        // If active, we load the counter output with 4'b0000
        if (reset.read()) {
            count = 0;
            // If enable is active, then we increment the counter
        } else if (enable.read()) {
            count++;
            cout << "@" << sc_time_stamp() << " :: Incremented Counter "
                 << counter_out.read() << endl;
        }
        counter_out.write(count);
    } // End of function incr_count

    // Constructor for the counter
    // Since this counter is a positive edge trigged one,
    // We trigger the below block with respect to positive
    // edge of the clock and also when ever reset changes state
    SC_CTOR(counter) {
        cout << "Executing new" << endl;
        SC_METHOD(incr_count);
        sensitive << reset;
        sensitive << clock.pos();
    } // End of Constructor

}; // End of Module counter