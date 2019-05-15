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
 * This class inherits the abstract sigutils::SignalIO base class and
 * implicitly overrides some non-virtual base methods to implement only the
 * receiving functionality.
 */
class ZMQReceiver : public SignalIO {

private:

    zmq::socket_t &m_socket;
    zmq::message_t m_buf;
    msgpack::unpacked m_unpacker;

public:

    // register the data container with MessagePack
    MSGPACK_DEFINE (m_data)

    /*
     * Default constructor - initializes the `zmq` sockets
     */
    explicit ZMQReceiver(int, zmq::socket_t &);

    /*
     * Receives data and unpacks the buffer to MessagePack
     */
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
 * This class inherits the abstract sigutils::SignalIO base class and
 * implicitly overrides some non-virtual base methods to implement only the
 * transmitting functionality.
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

    /*
     * Default constructor - initializes the `zmq` sockets
     */
    explicit ZMQTransmitter(int, zmq::socket_t &);

    /*
     * Destructor - clears any remaining MessagePack buffers
     */
    ~ZMQTransmitter();

    /*
     * Packs the buffer to MessagePack and sends the data
     */
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


inline ZMQReceiver::ZMQReceiver(const int num_ports, zmq::socket_t &socket) :
    SignalIO(num_ports), m_socket(socket) {
    // do nothing
}


inline void ZMQReceiver::recv() {

    m_socket.recv(&m_buf);
    msgpack::unpack(m_unpacker, (char *) (m_buf.data()), m_buf.size());
    m_unpacker.get().convert(*this);

}


/* -------------------- ZMQTRANSMITTER IMPLEMENTATIONS -------------------- */


inline ZMQTransmitter::ZMQTransmitter(const int num_ports, zmq::socket_t &socket) :
    SignalIO(num_ports), m_socket(socket), m_packer(&m_sbuf) {
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

#endif  // ZMQSIGS_HPP
