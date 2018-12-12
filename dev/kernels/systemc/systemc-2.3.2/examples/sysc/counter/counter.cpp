//-----------------------------------------------------
// This is my second Systemc Example
// Design Name : counter
// File Name : counter.cpp
// Function : This is a 4 bit up-counter with
// Synchronous active high reset and
// with active high enable signal
//-----------------------------------------------------
#include "systemc.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <sstream>

//template<typename T>
//std::ostream &operator<<(std::ostream &outs, const sc_out<T> &p) {
//    return outs << p;
//}

template<typename T>
std::string to_string(const T &value) {
    std::ostringstream ss;
    ss << value;
    return ss.str();
}

SC_MODULE (sysc_counter) {

    sc_in_clk clock;      // Clock input of the design
    sc_in<bool> reset;      // active high, synchronous Reset input
    sc_in<bool> enable;      // Active high enable signal for counter
    sc_out<sc_uint<8> > counter_out; // 4 bit vector output of the counter

    //------------Local Variables Here---------------------
    sc_uint<8> count;

    char buffer[256];
    int portno = 8080;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *server = gethostbyname("work-vm01");
    struct sockaddr_in serv_addr;


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
            strncpy(buffer, "CHILD", sizeof(buffer) - 1);

            int n = write(sockfd, to_string(counter_out).c_str(), strlen(buffer));
            if (n < 0)
                perror("ERROR writing to socket");
            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            if (n < 0)
                perror("ERROR reading from socket");

            printf("maybe? %s\n", buffer);

            cout << "@" << sc_time_stamp() << " :: Incremented Counter "
                 << to_string(counter_out) << endl;
        }
        counter_out.write(count);
    } // End of function incr_count

    // Constructor for the counter
    // Since this counter is a positive edge triggered one,
    // We trigger the below block with respect to positive
    // edge of the clock and also when ever reset changes state
    SC_CTOR(sysc_counter) {

        // -------------- SERVER-SIDE -----------------


        if (sockfd < 0) {
            perror("ERROR opening socket");
        }

        if (!server) {
            fprintf(stderr, "ERROR, no such host\n");
            exit(0);
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;

        bcopy(server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(portno);

        while (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR connecting");
            exit(-1);
        }

        // -------------- SERVER-SIDE -----------------

        cout << "Executing new" << endl;
        SC_METHOD(incr_count);
        sensitive << reset;
        sensitive << clock.pos();
    } // End of Constructor

    ~sysc_counter() {
        close(sockfd);
    }

}; // End of Module counter
