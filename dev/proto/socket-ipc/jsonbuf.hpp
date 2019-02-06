#ifndef JSON_BUF_HPP
#define JSON_BUF_HPP

#include "json.hpp"

using json = nlohmann::json;

#include <sstream>
#include <unistd.h>

// keep this just for dev
#include <iostream>

#define BUFSIZE 1025

template<typename T>
std::string to_string(const T &value) {

    std::ostringstream ss;
    ss << value;
    return ss.str();

}


void send_json(const json &data, int sock_fd) {

    // convert JSON object to bytes to be transmitted via sockets
    std::string data_str = to_string(data);
    std::cout << "CONVERTING: " << data_str << " SIZE: " << data_str.size() << std::endl;

    if (write(sock_fd, data_str.c_str(), data_str.size()) < 0) {
        perror("ERROR writing to socket");
    }

}

json recv_json(char buffer[], int sock_fd) {

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
        std::cout << getpid() << ' ';
        perror("JSON PARSE ERROR");
        exit(-1);
        // return json{};
    }

}

#endif
