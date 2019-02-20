#ifndef JSON_BUF_HPP
#define JSON_BUF_HPP

#include "json.hpp"
#include <mpi.h>
using json = nlohmann::json;

#include <sstream>
#include <unistd.h>

// keep this just for dev
#include <iostream>
#include "cprint.hpp"

Color::Modifier RED(Color::FG_RED);
Color::Modifier BLUE(Color::FG_BLUE);
Color::Modifier DEF(Color::FG_DEFAULT);

#define BUFSIZE 1025

template<typename T>
std::string to_string(const T &value) {

    std::ostringstream ss;
    ss << value;
    return ss.str();

}

void scatter_json(const json &data, char *send_buffer, char *recv_buffer,
        int num_procs, bool send, MPI_Comm comm) {

    for (int i = 0; i < num_procs; i++) {
        strncpy(&send_buffer[i], to_string(data[i]).c_str(), BUFSIZE);
    }

//    std::cout << &send_buffer[0] << std::endl;
//
//    if (send) {
//        MPI_Scatter(send_buffer, BUFSIZE, MPI_CHAR, nullptr, BUFSIZE, MPI_CHAR, MPI_ROOT, comm);
//        MPI_Gather(nullptr, BUFSIZE, MPI_CHAR, recv_buffer, BUFSIZE, MPI_CHAR, MPI_ROOT, comm);
//    }

}

//void send_json(const json &data, int sock_fd) {
//
//    // convert JSON object to bytes to be transmitted via sockets
//    std::string data_str = to_string(data);
//    ulong buf_size = data_str.size();
//
////    std::cout << getpid() << " SENDING BEFORE: " << data_str << std::endl;
//    if (write(sock_fd, data_str.c_str(), buf_size) != buf_size) {
//        perror("ERROR writing to socket");
//    }
//
//    data_str.clear();
////    std::cout << getpid() << " SENDING AFTER: " << data_str << std::endl;
//
//}

const json recv_json(char buffer[], int sock_fd) {

    ssize_t read_bytes = read(sock_fd, buffer, BUFSIZE);

    if (read_bytes > 0) {

        try {

            buffer[read_bytes] = '\0';
            json parsed = json::parse(buffer);
            std::cout << "SENDING " << parsed << " TO " << sock_fd << std::endl;
            return parsed;

        } catch (json::parse_error &e) {

            std::cout << BLUE << getpid() << DEF << " JSON PARSE ERROR " << e.what() << buffer << std::endl;
            bzero(buffer, BUFSIZE);
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
