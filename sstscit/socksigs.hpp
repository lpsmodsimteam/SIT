/*
 * SocketSignal class definitions and implementations.
 */

#ifndef SOCKSIGS_HPP
#define SOCKSIGS_HPP

#define BUFSIZE 500

#include "sigutils.hpp"

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

    /*
     * Default constructor - initializes the sockets and buffers. The default
     * parameter `server_side` is set to true for parent processes. The child
     * processes need to set the parameter to false to set up the connection
     * properly.
     */
    explicit SocketSignal(int, bool = true);

    /*
     * Destructor - unlinks and closes the sockets after use
     */
    ~SocketSignal();

    void set_addr(const std::string &addr);

    /*
     * Packs the buffer to MessagePack and sends the data
     */
    void send();

    /*
     * Receives data and unpacks the buffer to MessagePack
     */
    void recv();

};


/* -------------------- SIGNALRECEIVER IMPLEMENTATIONS -------------------- */

inline SocketSignal::SocketSignal(int socket, bool server_side) :
    m_server_side(server_side), m_socket(socket), m_rd_socket(0),
    m_rd_bytes(0), m_buf(""), m_addr({}), m_packer(&m_sbuf) {
    // do nothing
}

inline SocketSignal::~SocketSignal() {

    unlink(m_addr.sun_path);
    close(m_socket);
    close(m_rd_socket);

}

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

inline void SocketSignal::send() {

    m_packer.pack(*this);

    (m_server_side) ? write(m_rd_socket, m_sbuf.data(), m_sbuf.size())
                    : write(m_socket, m_sbuf.data(), m_sbuf.size());

    m_sbuf.clear();

}

inline void SocketSignal::recv() {

    m_rd_bytes = static_cast<size_t>(
        (m_server_side) ? read(m_rd_socket, m_buf, BUFSIZE) :
        read(m_socket, m_buf, BUFSIZE));

    m_buf[m_rd_bytes] = '\0';

    msgpack::unpack(m_unpacker, m_buf, m_rd_bytes);
    m_unpacker.get().convert(*this);

}

#endif
