/*
 * SocketSignal class definitions and implementations.
 */

#ifndef SOCKSIGS_HPP
#define SOCKSIGS_HPP

// default buffer size
#ifndef BUFSIZE
#define BUFSIZE 15
#endif

// default buffer size
#ifndef MSGPACK
#include "sigutilslite.hpp"
#else
#include "sigutils.hpp"
#endif

#include <sys/un.h>

/*
 * Implements methods to receive signals via UNIX domain sockets.
 *
 * This class inherits the abstract sigutils::SignalIO base class.
 */
class SocketSignal : public SignalIO {

private:

    // flag to determine the options for setting up the sockets
    bool m_server_side;
    int m_socket, m_rd_socket;
    size_t m_rd_bytes;
    char m_buf[BUFSIZE];
    struct sockaddr_un m_addr;

    msgpack::unpacked m_unpacker;
    msgpack::packer<msgpack::sbuffer> m_packer;
    msgpack::sbuffer m_sbuf;

public:

    // register the data container with MessagePack
    MSGPACK_DEFINE (m_data)

    explicit SocketSignal(int, int, bool = true);

    ~SocketSignal();

    void set_addr(const std::string &);

    void send();

    void recv();

};


/* -------------------- SIGNALRECEIVER IMPLEMENTATIONS -------------------- */

/*
 * Initializes the sockets, buffers and MessagePack variables
 *
 * Arguments:
 *     num_ports -- Number of ports used in the black box interface. This number is usually 1
 *                  greater than the total number of the SystemC module ports
 *     socket -- Unix domain socket
 *     server_side (default: true) -- Flag is set to true for parent processes. The child processes
 *                                    need to set the parameter to false to set up the connection
 *                                    properly.
 */
inline SocketSignal::SocketSignal(const int num_ports, int socket, bool server_side) :
    SignalIO(num_ports), m_server_side(server_side), m_socket(socket), m_rd_socket(0),
    m_rd_bytes(0), m_buf(""), m_addr({}), m_packer(&m_sbuf) {
    // do nothing
}

/*
 * Unlinks and closes the sockets after use
 */
inline SocketSignal::~SocketSignal() {

    unlink(m_addr.sun_path);
    close(m_socket);
    close(m_rd_socket);

}

/*
 * Sets configuration options for sockets
 *
 * Arguments:
 *     addr -- Unix domain socket address
 */
inline void SocketSignal::set_addr(const std::string &addr) {

    if (m_socket < 0) {
        perror("Socket creation\n");
    }

    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    strcpy(m_addr.sun_path, addr.c_str());

    // parent process socket options
    if (m_server_side) {

        if (bind(m_socket, (struct sockaddr *) &m_addr, sizeof(m_addr)) < 0) {
            perror("Bind failed\n");
        }

        if (listen(m_socket, 5) < 0) {
            perror("Socket listen\n");
        }

        socklen_t addr_len = sizeof(m_addr);
        if ((m_rd_socket = accept(m_socket, (struct sockaddr *) &m_addr, &addr_len)) < 0) {
            perror("Accept failed\n");
        }

    } else {  // child process socket options

        if (connect(m_socket, (struct sockaddr *) &m_addr, sizeof(m_addr)) < 0) {
            perror("Connection failed\n");
        }

    }

}

/*
 * Packs the buffer to MessagePack and sends the data
 */
inline void SocketSignal::send() {

    m_packer.pack(*this);

    (m_server_side) ? write(m_rd_socket, m_sbuf.data(), m_sbuf.size())
                    : write(m_socket, m_sbuf.data(), m_sbuf.size());

    m_sbuf.clear();

}

/*
 * Receives data and unpacks the buffer to MessagePack.
 *
 * Throws a msgpack::insufficient_bytes exception during runtime if the buffer size is
 * insufficient.
 */
inline void SocketSignal::recv() {

    try {

        m_rd_bytes = static_cast<size_t>(
            (m_server_side) ? read(m_rd_socket, m_buf, BUFSIZE) :
            read(m_socket, m_buf, BUFSIZE));

        m_buf[m_rd_bytes] = '\0';

        msgpack::unpack(m_unpacker, m_buf, m_rd_bytes);
        m_unpacker.get().convert(*this);

    } catch (msgpack::v1::insufficient_bytes &) {

        perror("\033[31mInsufficient size for transfer buffers. Try increasing BUFSIZE\033[0m\n");
        throw;

    }

}

#endif
