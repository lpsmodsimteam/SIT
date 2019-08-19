/*
 * ZMQReceiver and ZMQTransmitter class definitions and implementations.
 */

#ifndef ZMQSIGS_HPP
#define ZMQSIGS_HPP

#include "sigutils.hpp"

#include <zmq.hpp>

/*
 * Implements methods to receive signals via ZeroMQ.
 *
 * This class inherits the abstract sigutils::SignalIO base class and implicitly overrides some
 * non-virtual base methods to implement only the receiving functionality.
 */
class ZMQReceiver : public SignalIO {

private:

    zmq::socket_t &m_socket;
    zmq::message_t m_buf;
    msgpack::unpacked m_unpacker;

public:

    // register the data container with MessagePack
    MSGPACK_DEFINE (m_data)

    explicit ZMQReceiver(int, zmq::socket_t &);

    void recv();

    // hide non-virtual base method
    void set_state(bool) {};

    // hide non-virtual base method
    template<typename T>
    void set(const std::string &, const T &) {}

};

/*
 * Implements methods to transmit signals via ZeroMQ.
 *
 * This class inherits the abstract sigutils::SignalIO base class and implicitly overrides some
 * non-virtual base methods to implement only the transmitting functionality.
 */
class ZMQTransmitter : public SignalIO {

private:

    zmq::socket_t &m_socket;
    zmq::message_t m_buf;
    msgpack::packer<msgpack::sbuffer> m_packer;
    msgpack::sbuffer m_sbuf;

public:

    // register the data container with MessagePack
    MSGPACK_DEFINE (m_data)

    explicit ZMQTransmitter(int, zmq::socket_t &);

    ~ZMQTransmitter();

    void send();

    // hide non-virtual base method
    template<typename T>
    T get(const std::string &) {}

    // hide non-virtual base method
    bool alive() { return false; }

    // hide non-virtual base method
    bool get_clock_pulse(const std::string &) { return false; }

};


/* -------------------- ZMQRECEIVER IMPLEMENTATIONS -------------------- */

/*
 * Initializes the ZeroMQ sockets
 *
 * Arguments:
 *     num_ports -- Number of ports used in the black box interface. This number is usually 1
 *                  greater than the total number of the SystemC module ports
 *     socket -- ZeroMQ socket
 */
inline ZMQReceiver::ZMQReceiver(const int num_ports, zmq::socket_t &socket) :
    SignalIO(num_ports), m_socket(socket) {
    // do nothing
}

/*
 * Receives data and unpacks the buffer to MessagePack
 */
inline void ZMQReceiver::recv() {

    m_socket.recv(&m_buf);
    msgpack::unpack(m_unpacker, (char *) (m_buf.data()), m_buf.size());
    m_unpacker.get().convert(*this);

}


/* -------------------- ZMQTRANSMITTER IMPLEMENTATIONS -------------------- */


/*
 * Initializes the ZeroMQ sockets
 *
 * Arguments:
 *     num_ports -- Number of ports used in the black box interface. This number is usually 1
 *                  greater than the total number of the SystemC module ports
 *     socket -- ZeroMQ socket
 */
inline ZMQTransmitter::ZMQTransmitter(const int num_ports, zmq::socket_t &socket) :
    SignalIO(num_ports), m_socket(socket), m_packer(&m_sbuf) {
    // do nothing
}

/*
 * Clears any remaining MessagePack buffers
 */
inline ZMQTransmitter::~ZMQTransmitter() {

    m_sbuf.clear();

}

/*
 * Packs the buffer to MessagePack and sends the data
 */
inline void ZMQTransmitter::send() {

    m_packer.pack(*this);
    m_buf.rebuild(m_sbuf.size());
    std::memcpy(m_buf.data(), m_sbuf.data(), m_sbuf.size());
    m_socket.send(m_buf);
    m_sbuf.clear();

}

#endif  // ZMQSIGS_HPP
