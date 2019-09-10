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
    zmq::message_t m_msg;
    char m_buf[BUFSIZE];

public:

    explicit ZMQReceiver(zmq::socket_t &);

    void recv();

    // hide non-virtual base method
    void set(const std::string&) {}

    // hide non-virtual base method
    void set_state(bool) {};

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
    zmq::message_t m_msg;

public:

    explicit ZMQTransmitter(zmq::socket_t &);

    ~ZMQTransmitter();

    void send();

    // hide non-virtual base method
    std::string get() {};

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
inline ZMQReceiver::ZMQReceiver(zmq::socket_t &socket) :
    SignalIO(), m_socket(socket) {
    // do nothing
}

/*
 * Receives data and unpacks the buffer to MessagePack
 */
inline void ZMQReceiver::recv() {

    m_socket.recv(&m_msg);
    memcpy(m_buf, m_msg.data(), m_msg.size());
    m_buf[m_msg.size()] = '\0';
    m_data = m_buf;

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
inline ZMQTransmitter::ZMQTransmitter(zmq::socket_t &socket) :
    SignalIO(), m_socket(socket) {
    // do nothing
}

/*
 * Clears any remaining MessagePack buffers
 */
inline ZMQTransmitter::~ZMQTransmitter() {}

/*
 * Packs the buffer to MessagePack and sends the data
 */
inline void ZMQTransmitter::send() {

    m_msg.rebuild(m_data.size());
    std::memcpy(m_msg.data(), m_data.c_str(), m_data.size());
    m_socket.send(m_msg);

}

#endif  // ZMQSIGS_HPP
