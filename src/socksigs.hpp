#ifndef SOCKSIGS_HPP
#define SOCKSIGS_HPP

#define BUFSIZE 4096

#include "sigutils.hpp"

#include <sys/un.h>


class SignalSocket : public SignalIO {

private:

    bool m_server_side;
    int m_socket, m_rd_socket;
    size_t m_rd_bytes;
    struct sockaddr_un m_addr;
    char rd_buf[BUFSIZE];

    msgpack::unpacked m_unpacker;
    msgpack::packer<msgpack::sbuffer> m_packer;
    msgpack::sbuffer m_sbuf;

public:

    MSGPACK_DEFINE (m_data);

    explicit SignalSocket(int, bool = true);

    ~SignalSocket();

    void set_addr(const std::string &addr);

    void send();

    void recv();

};


/* -------------------- SIGNALRECEIVER IMPLEMENTATIONS -------------------- */

inline SignalSocket::SignalSocket(int socket, bool server_side) :
    m_socket(socket), m_server_side(server_side), m_packer(&m_sbuf),
    m_rd_socket(0), m_rd_bytes(0), m_addr({}), rd_buf("") {
    // do nothing
}

inline SignalSocket::~SignalSocket() {

    unlink(m_addr.sun_path);
    close(m_socket);
    close(m_rd_socket);

}

inline void SignalSocket::set_addr(const std::string &addr) {

    if (m_socket < 0) {
        perror("Socket creation\n");
    }

    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    strcpy(m_addr.sun_path, addr.c_str());

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

    } else {

        if (connect(m_socket, (struct sockaddr *) &m_addr, sizeof(m_addr)) < 0) {
            perror("Connection failed\n");
        }

    }

}

inline void SignalSocket::send() {

    m_packer.pack(*this);

    (m_server_side) ? write(m_rd_socket, m_sbuf.data(), m_sbuf.size())
                    : write(m_socket, m_sbuf.data(), m_sbuf.size());

    m_sbuf.clear();

}

inline void SignalSocket::recv() {

    m_rd_bytes = static_cast<size_t>(
        (m_server_side) ? read(m_rd_socket, rd_buf, BUFSIZE) : read(m_socket, rd_buf, BUFSIZE));

    rd_buf[m_rd_bytes] = '\0';
    msgpack::unpack(m_unpacker, rd_buf, m_rd_bytes);
    m_unpacker.get().convert(*this);

}

#endif
