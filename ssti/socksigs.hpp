/*
 * SocketSignal class definitions and implementations.
 */

#ifndef SOCKSIGS
#define SOCKSIGS

#include "sigutils.hpp"

#include <sys/socket.h>
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
    struct sockaddr_un m_addr;

public:

    explicit SocketSignal(int, bool = true);

    ~SocketSignal();

    void set_addr(const std::string &);

    void send() override;

    void recv() override;

};


/* -------------------- SOCKETSIGNAL IMPLEMENTATIONS -------------------- */

/*
 * Initializes the sockets, buffers and MessagePack variables
 *
 * Arguments:
 *     socket -- Unix domain socket
 *     server_side (default: true) -- Flag is set to true for parent processes. The child processes
 *                                    need to set the parameter to false to set up the connection
 *                                    properly.
 */
inline SocketSignal::SocketSignal(int buf_size, bool server_side) :
    SignalIO(buf_size), m_server_side(server_side),
    m_socket(socket(AF_UNIX, SOCK_STREAM, 0)),
    m_rd_socket(0), m_addr({}) {}

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
 * Packs the buffer and sends the data
 */
inline void SocketSignal::send() {

    write((m_server_side ? m_rd_socket : m_socket), m_data, strlen(m_data));

}

/*
 * Receives data and unpacks the buffer
 *
 */
inline void SocketSignal::recv() {

    m_data[read((m_server_side ? m_rd_socket : m_socket), m_data, m_buf_size)] = '\0';

}

#endif  // SOCKSIGS
