#ifndef JSON_BUF
#define JSON_BUF

#include "json.hpp"

using json = nlohmann::json;

#define BUFSIZE 256

#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

void send_signal(const json &, int);

json recv_signal(char [], int);

template<typename T>
std::string to_string(const T &value);

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

#endif
