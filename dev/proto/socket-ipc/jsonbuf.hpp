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

//    std::cout << getpid() << " SENDING BEFORE: " << data_str << std::endl;
    if (write(sock_fd, data_str.c_str(), buf_size) != buf_size) {
        perror("ERROR writing to socket");
    }

//    data.clear();
    data_str.clear();
//    std::cout << getpid() << " SENDING AFTER: " << data_str << std::endl;

}

json recv_json(char buffer[], int sock_fd) {

    ssize_t read_bytes = read(sock_fd, buffer, BUFSIZE);

    if (read_bytes > 0) {

        try {

            buffer[read_bytes] = '\0';
            return json::parse(buffer);

        } catch (json::parse_error &e) {

            std::cout << getpid() << " JSON PARSE ERROR " << e.what() << buffer << std::endl;
            return json{};

        } catch (json::type_error &e) {

            std::cout << getpid() << " JSON TYPE ERROR " << e.what() << buffer << std::endl;
            return json{};

        }

    } else if (read_bytes < 0) {

        std::cout << getpid() << " BUFFER READ ERROR" << std::endl;
        perror("BUFFER READ ERROR");

    } else {

        std::cout << getpid() << " EMPTY BUFFER" << std::endl;
        perror("EMPTY BUFFER");

    }

    return json{};

}

#endif
