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
    ulong buf_size = data_str.size();

    if (write(sock_fd, data_str.c_str(), buf_size) != buf_size) {
        perror("ERROR writing to socket");
    }

}

json recv_json(char buffer[], int sock_fd) {

    ssize_t valread = read(sock_fd, buffer, BUFSIZE);
    if (valread > 0) {
        buffer[valread] = '\0';
        try {
            return json::parse(buffer);
        } catch (json::parse_error &e) {
            perror("JSON PARSE ERROR");
            return json{};
        }
    }

    printf("YOOOOOOOOOOOOOOOOO %zd\n", valread);
//    return json{};

}

#endif
