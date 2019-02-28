#ifndef JSON_BUF_HPP
#define JSON_BUF_HPP

#include "json.hpp"

using json = nlohmann::json;

#include <sstream>
#include <unistd.h>

#define BUFSIZE 1025

/* -------------------- DECLARATIONS -------------------- */

template<typename T>
std::string _to_string(const T &);

template<typename T>
int _sc_signal_to_int(const T &);

//void transmit_signals(const char *, MPI_Comm, bool = true);
//
//void receive_signals(char *, MPI_Comm, bool = true);
//
//void init_MPI();

/* -------------------- IMPLEMENTATIONS -------------------- */

template<typename T>
std::string _to_string(const T &value) {

    std::ostringstream ss;
    ss << value;
    return ss.str();

}

template<typename T>
int _sc_signal_to_int(const T &value) {

    return std::stoi(_to_string(value));

}

//void init_MPI() {
//
//    int done_already;
//    MPI_Initialized(&done_already);
//    if (!done_already) {
//        MPI_Init(nullptr, nullptr);
//    }
//}
//
//void transmit_signals(const char *send_buffer, MPI_Comm inter_com, bool from_parent) {
//
//    if (from_parent) {
//
//        MPI_Scatter(send_buffer, BUFSIZE, MPI_CHAR, nullptr, BUFSIZE, MPI_CHAR, MPI_ROOT, inter_com);
//
//    } else {
//
//        MPI_Gather(send_buffer, BUFSIZE, MPI_CHAR, nullptr, BUFSIZE, MPI_CHAR, 0, inter_com);
//
//    }
//
//}
//
//void receive_signals(char *receive_buffer, MPI_Comm inter_com, bool from_parent) {
//
//    if (from_parent) {
//
//        MPI_Gather(nullptr, BUFSIZE, MPI_CHAR, receive_buffer, BUFSIZE, MPI_CHAR, MPI_ROOT, inter_com);
//
//    } else {
//
//        MPI_Scatter(nullptr, BUFSIZE, MPI_CHAR, receive_buffer, BUFSIZE, MPI_CHAR, 0, inter_com);
//
//    }
//
//}

#endif
