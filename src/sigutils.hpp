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
    zmq::message_t m_buf;
    msgpack::packer<msgpack::sbuffer> m_packer;
    msgpack::unpacked m_unpacker;
    msgpack::sbuffer m_sbuf;
    std::unordered_map<std::string, std::string> m_data;

public:

    MSGPACK_DEFINE (m_data);

    explicit SignalHandler(zmq::socket_t &);

    ~SignalHandler();

    void send();

    void recv();

    std::string &operator[](std::string);

};

/* -------------------- IMPLEMENTATIONS -------------------- */

SignalHandler::SignalHandler(zmq::socket_t &socket) :
        m_socket(socket), m_packer(&m_sbuf) {
    // do nothing
}

SignalHandler::~SignalHandler() {

    m_data.clear();
    m_sbuf.clear();

}

std::string &SignalHandler::operator[](const std::string index) {

    return m_data[index];

}


void SignalHandler::send() {

    m_packer.pack(*this);
    m_buf.rebuild(m_sbuf.size());
    std::memcpy(m_buf.data(), m_sbuf.data(), m_sbuf.size());
    m_socket.send(m_buf);
    m_sbuf.clear();

}

void SignalHandler::recv() {

    m_socket.recv(&m_buf);
    msgpack::unpack(m_unpacker, (char *) (m_buf.data()), m_buf.size());
    m_unpacker.get().convert(*this);

}

/* -------------------- DECLARATIONS -------------------- */

template<typename T>
std::string _to_string(const T &);

template<typename T>
int _sc_signal_to_int(const T &);


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
