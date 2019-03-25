#ifndef ZMQSIGS_HPP
#define ZMQSIGS_HPP

#include "sigutils.hpp"
#include <zmq.hpp>


class ZMQReceiver : public SignalIO {

private:

    zmq::socket_t &m_socket;
    zmq::message_t m_buf;
    msgpack::unpacked m_unpacker;

public:

    MSGPACK_DEFINE (m_data)

    explicit ZMQReceiver(zmq::socket_t &);

    void recv();

    // hide non-virtual base method
    void set_state(bool) {};

    // hide non-virtual base method
    template<typename T>
    void set(const std::string &, const T &) {};


};

class ZMQTransmitter : public SignalIO {

private:

    zmq::socket_t &m_socket;
    zmq::message_t m_buf;
    msgpack::packer<msgpack::sbuffer> m_packer;
    msgpack::sbuffer m_sbuf;

public:

    MSGPACK_DEFINE (m_data);

    explicit ZMQTransmitter(zmq::socket_t &);

    ~ZMQTransmitter();

    void send();

    // hide non-virtual base method
    template<typename T>
    T get(const std::string &) {};

    // hide non-virtual base method
    bool alive() { return false; };

    // hide non-virtual base method
    bool get_clock_pulse(const std::string &) { return false; };


};


/* -------------------- ZMQRECEIVER IMPLEMENTATIONS -------------------- */


inline ZMQReceiver::ZMQReceiver(zmq::socket_t &socket) :
    m_socket(socket) {
    // do nothing
}


inline void ZMQReceiver::recv() {

    m_socket.recv(&m_buf);
    msgpack::unpack(m_unpacker, (char *) (m_buf.data()), m_buf.size());
    m_unpacker.get().convert(*this);

}


/* -------------------- ZMQTRANSMITTER IMPLEMENTATIONS -------------------- */


inline ZMQTransmitter::ZMQTransmitter(zmq::socket_t &socket) :
    m_socket(socket), m_packer(&m_sbuf) {
    // do nothing
}

inline ZMQTransmitter::~ZMQTransmitter() {

    m_sbuf.clear();

}

inline void ZMQTransmitter::send() {

    m_packer.pack(*this);
    m_buf.rebuild(m_sbuf.size());
    std::memcpy(m_buf.data(), m_sbuf.data(), m_sbuf.size());
    m_socket.send(m_buf);
    m_sbuf.clear();

}

#endif
