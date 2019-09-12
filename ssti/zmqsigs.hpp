/*
 * ZMQSignal and ZMQTransmitter class definitions and implementations.
 */

#ifndef ZMQSIGS
#define ZMQSIGS

// default buffer size
#ifndef BUFSIZE
#define BUFSIZE 5
#endif

#include "sigutils.hpp"

#include <zmq.hpp>

/*
 * Implements methods to receive signals via ZeroMQ.
 *
 * This class inherits the abstract sigutils::SignalIO base class and implicitly overrides some
 * non-virtual base methods to implement only the receiving functionality.
 */
class ZMQSignal : public SignalIO {

private:

    bool m_server_side;
    zmq::socket_t &m_socket;
    zmq::message_t m_msg;

public:

    explicit ZMQSignal(zmq::socket_t &, bool = true);

    ~ZMQSignal();

    void recv();

    void send();

    void set_addr(const std::string &);
};


/* -------------------- ZMQSIGNAL IMPLEMENTATIONS -------------------- */

/*
 * Initializes the ZeroMQ sockets
 *
 * Arguments:
 *     num_ports -- Number of ports used in the black box interface. This number is usually 1
 *                  greater than the total number of the SystemC module ports
 *     socket -- ZeroMQ socket
 */
inline ZMQSignal::ZMQSignal(zmq::socket_t &socket, bool server_side) :
    SignalIO(), m_server_side(server_side), m_socket(socket) {
}

inline ZMQSignal::~ZMQSignal() {

    m_socket.close();

}


inline void ZMQSignal::set_addr(const std::string &addr) {

    (m_server_side) ? m_socket.connect(addr) : m_socket.bind(addr);
}

/*
 * Receives data and unpacks the buffer to MessagePack
 */
inline void ZMQSignal::recv() {

    auto _buf = new char[BUFSIZE]{};
    m_socket.recv(&m_msg);
    memcpy(_buf, m_msg.data(), m_msg.size());
    _buf[m_msg.size()] = '\0';
    *m_data = _buf;
    delete[] _buf;

}

/*
 * Packs the buffer to MessagePack and sends the data
 */
inline void ZMQSignal::send() {

    m_msg.rebuild(m_data->size());
    std::memcpy(m_msg.data(), m_data->c_str(), m_data->size());
    m_socket.send(m_msg);

}

#endif  // ZMQSIGS
