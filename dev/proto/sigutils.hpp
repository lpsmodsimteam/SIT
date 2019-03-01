#ifndef SIGUTILS_HPP
#define SIGUTILS_HPP

#include <msgpack.hpp>

#include <sstream>
#include <unistd.h>
#include <unordered_map>

struct signal_packet {

    std::unordered_map<std::string, std::string> data;
    MSGPACK_DEFINE (data);

};

/* -------------------- DECLARATIONS -------------------- */

template<typename T>
std::string _to_string(const T &);

template<typename T>
int _sc_signal_to_int(const T &);

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

#endif
