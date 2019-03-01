#ifndef SIGUTILS_HPP
#define SIGUTILS_HPP

#include <msgpack.hpp>
#include <zmq.hpp>

#include <sstream>
#include <unistd.h>
#include <unordered_map>

class SignalHandler {

private:

    zmq::socket_t &m_socket;
    std::unordered_map<std::string, std::string> m_data;

public:

    MSGPACK_DEFINE (m_data);
    explicit SignalHandler(zmq::socket_t &);

    ~SignalHandler();

    std::string &operator[](std::string);


};

SignalHandler::SignalHandler(zmq::socket_t &socket) : m_socket(socket) {

}

SignalHandler::~SignalHandler() {

    m_data.clear();

}

std::string &SignalHandler::operator[](const std::string index) {

    return m_data[index];

}

/* -------------------- DECLARATIONS -------------------- */

void send_sigs(zmq::socket_t &, msgpack::packer<msgpack::sbuffer> &,
               msgpack::sbuffer &, zmq::message_t &, SignalHandler &);

void recv_sigs(zmq::socket_t &, msgpack::unpacked &, zmq::message_t &,
               SignalHandler &);

template<typename T>
std::string _to_string(const T &);

template<typename T>
int _sc_signal_to_int(const T &);

/* -------------------- IMPLEMENTATIONS -------------------- */


void send_sigs(zmq::socket_t &socket, msgpack::packer<msgpack::sbuffer> &packer,
               msgpack::sbuffer &sbuf, zmq::message_t &buf_out, SignalHandler &data_out) {

    packer.pack(data_out);
    buf_out.rebuild(sbuf.size());
    std::memcpy(buf_out.data(), sbuf.data(), sbuf.size());
    socket.send(buf_out);
    sbuf.clear();

}

void recv_sigs(zmq::socket_t &socket, msgpack::unpacked &unpacker, zmq::message_t &buf_in,
               SignalHandler &data_in) {

    socket.recv(&buf_in);
    msgpack::unpack(unpacker, (char *) (buf_in.data()), buf_in.size());
    unpacker.get().convert(data_in);

}


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
