/*
 * ZMQSignal class definitions and implementations.
 */

#ifndef ZMQSIGS
#define ZMQSIGS

#include <zmq.hpp>

#include "sigutils.hpp"

/*
 * Implements methods to receive signals via ZeroMQ.
 *
 * This class inherits the abstract sigutils::SignalIO base class and implicitly
 * overrides some non-virtual base methods to implement only the receiving
 * functionality.
 */
class ZMQSignal : public SignalIO {
private:
    bool m_server_side;
    zmq::context_t m_context;
    zmq::socket_t m_socket;
    zmq::message_t m_msg;

public:
    explicit ZMQSignal(int, bool = true);

    ~ZMQSignal();

    void set_addr(const std::string&);

    void set_addr(const std::string&, const std::string&) = delete;

    void recv() override;

    void send() override;
};

/* -------------------- ZMQSIGNAL IMPLEMENTATIONS -------------------- */

/*
 * Initializes the ZeroMQ sockets
 *
 * Arguments:
 *     num_ports -- Number of ports used in the black box interface. This number
 * is usually 1 greater than the total number of the SystemC module ports
 */
inline ZMQSignal::ZMQSignal(int buf_size, bool server_side)
    : SignalIO(buf_size), m_server_side(server_side), m_context(1),
      m_socket(m_context, (m_server_side ? ZMQ_REP : ZMQ_REQ)) {}

inline ZMQSignal::~ZMQSignal() {
    m_socket.close();
}

inline void ZMQSignal::set_addr(const std::string& addr) {
    (m_server_side) ? m_socket.connect(addr) : m_socket.bind(addr);
}

/*
 * Receives data and unpacks the buffer
 */
inline void ZMQSignal::recv() {
    m_socket.recv(m_msg, zmq::recv_flags::none);
    memcpy(m_data, m_msg.data(), m_msg.size());
    m_data[m_msg.size()] = '\0';
}

/*
 * Packs the buffer and sends the data
 */
inline void ZMQSignal::send() {
    m_msg.rebuild(strlen(m_data));
    std::memcpy(m_msg.data(), m_data, strlen(m_data));
    m_socket.send(m_msg, zmq::send_flags::none);
}

#endif // ZMQSIGS
