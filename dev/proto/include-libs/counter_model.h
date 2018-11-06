#ifndef _counter_model_H
#define _counter_model_H


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "systemc.h"


SC_MODULE (_sc_counter) {

    sc_in_clk clock;      // Clock input of the design
    sc_in<bool> reset;      // active high, synchronous Reset input
    sc_in<bool> enable;      // Active high enable signal for counter
    sc_out<sc_uint<8> > counter_out; // 4 bit vector output of the counter

    //------------Local Variables Here---------------------
    sc_uint<8> count;

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
                 << counter_out.read() << '\n';
        }
        counter_out.write(count);

        // DEV ////////////////////////////////////

        int sockfd, newsockfd, portno;
        socklen_t clilen;
        char buffer[256];
        struct sockaddr_in serv_addr, cli_addr;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("ERROR opening socket");
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));
        portno = 8000;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);
        if (bind(sockfd, (struct sockaddr *) &serv_addr,
                 sizeof(serv_addr)) < 0) {
            perror("ERROR on binding");
        }

        listen(sockfd, 5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd,
                           (struct sockaddr *) &cli_addr,
                           &clilen);
        if (newsockfd < 0) {
            perror("ERROR on accept");
        }

        bzero(buffer, 256);
        int n = read(newsockfd, buffer, 255);

        if (n < 0) {
            perror("ERROR reading from socket");
        }
        printf("Here is the message: %s\n", count);

        n = write(newsockfd, "I got your message", 18);

        if (n < 0) {
            perror("ERROR writing to socket");
        }

        close(newsockfd);
        close(sockfd);

        // DEV /////////////////////////////////////

    } // End of function incr_count

    // Constructor for the counter
    // Since this counter is a positive edge trigged one,
    // We trigger the below block with respect to positive
    // edge of the clock and also when ever reset changes state
    SC_CTOR(_sc_counter) {
        cout << "Executing new\n";
        SC_METHOD(incr_count);
        sensitive << reset;
        sensitive << clock.pos();
    } // End of Constructor

}; // End of Module counter

int sc_test() {

    sc_signal<bool> clock;
    sc_signal<bool> reset;
    sc_signal<bool> enable;
    sc_signal<sc_uint<8> > counter_out;

    // Connect the DUT
    _sc_counter _sc_counter("COUNTER");
    _sc_counter.clock(clock);
    _sc_counter.reset(reset);
    _sc_counter.enable(enable);
    _sc_counter.counter_out(counter_out);

    sc_start(1, SC_NS);

    // Initialize all variables
    reset = 0;       // initial value of reset
    enable = 0;      // initial value of enable
    for (int i = 0; i < 2; i++) {
        clock = 0;
        sc_start(1, SC_NS);
        clock = 1;
        sc_start(1, SC_NS);
    }
    reset = 1;    // Assert the reset
    cout << "@" << sc_time_stamp() << " Asserting reset\n" << '\n';
    for (int i = 0; i < 2; i++) {
        clock = 0;
        sc_start(1, SC_NS);
        clock = 1;
        sc_start(1, SC_NS);
    }
    reset = 0;    // De-assert the reset
    cout << "@" << sc_time_stamp() << " De-Asserting reset\n" << '\n';
    for (int i = 0; i < 2; i++) {
        clock = 0;
        sc_start(1, SC_NS);
        clock = 1;
        sc_start(1, SC_NS);
    }
    cout << "@" << sc_time_stamp() << " Asserting Enable\n" << '\n';
    enable = 1;  // Assert enable
    for (int i = 0; i < 300; i++) {
        clock = 0;
        sc_start(1, SC_NS);
        clock = 1;
        sc_start(1, SC_NS);
    }
    cout << "@" << sc_time_stamp() << " De-Asserting Enable\n" << '\n';
    enable = 0; // De-assert enable

    cout << "@" << sc_time_stamp() << " Terminating simulation\n" << '\n';

    return EXIT_SUCCESS;  // Terminate simulation

}

#endif
