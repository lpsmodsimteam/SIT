#ifndef JSON_BUF_HPP
#define JSON_BUF_HPP

#include "json.hpp"
#include <mpi.h>
using json = nlohmann::json;

#include <sstream>
#include <unistd.h>

#define BUFSIZE 1025

template<typename T>
std::string to_string(const T &value) {

    std::ostringstream ss;
    ss << value;
    return ss.str();

}

#endif
