#ifndef SOCKETS_HPP
#define SOCKETS_HPP

#include "sigutils.hpp"

#include <msgpack.hpp>

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define BUFSIZE 1024

template<typename T>
std::string _to_string(const T &);

template<typename T>
std::string _to_string(const T &value) {

    std::ostringstream ss;
    ss << value;
    return ss.str();

}


class SignalSocket {

private:

    bool m_server_side;
    int m_socket, m_rd_socket;
    size_t m_rd_bytes;
    struct sockaddr_un m_addr;
    char rd_buf[BUFSIZE];

    msgpack::unpacked m_unpacker;
    msgpack::packer<msgpack::sbuffer> m_packer;
    msgpack::sbuffer m_sbuf;
    std::unordered_map<std::string, std::pair<std::string, uint8_t >> m_data;

public:

    MSGPACK_DEFINE (m_data);

    explicit SignalSocket(int, bool = true);

    ~SignalSocket();

    void set_params(const std::string &);

    template<typename T>
    T get(const std::string &);

    void recv();

    bool alive();

    // Converts SST clock cycles to pulses for SystemC modules
    bool get_clock_pulse(const std::string &);

    void set_state(bool);

    template<typename T>
    void set(const std::string &, const T &, uint8_t = SC_BIT_T);

    void send();

};


/* -------------------- SIGNALRECEIVER IMPLEMENTATIONS -------------------- */

inline SignalSocket::SignalSocket(int socket, bool master) :
    m_socket(socket), m_server_side(master), m_packer(&m_sbuf),
    m_rd_socket(0), m_rd_bytes(0), m_addr({}), rd_buf("") {
    // do nothing
}

inline SignalSocket::~SignalSocket() {

    unlink(m_addr.sun_path);
    close(m_socket);
    close(m_rd_socket);
    m_data.clear();

}

inline void SignalSocket::set_params(const std::string &addr) {

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

inline bool SignalSocket::get_clock_pulse(const std::string &key) {

    return (this->get<int>(key)) % 2;

}

inline bool SignalSocket::alive() {

    return (this->get<bool>("__on__"));

}

inline void SignalSocket::set_state(bool state) {

    this->set("__on__", state);

}

template<typename T>
T SignalSocket::get(const std::string &key) {

    std::string value = m_data[key].first;
    uint8_t data_t = m_data[key].second;

    switch (data_t) {
        case SC_BIT_T:
            return value != "0";
        case SC_UINT_T:
            return std::stoi(value);
        default:
            throw std::invalid_argument("INVALID TYPE OR SOMETHING");

    }

}

inline void SignalSocket::recv() {

    m_rd_bytes = static_cast<size_t>(
        (m_server_side) ? read(m_rd_socket, rd_buf, BUFSIZE) : read(m_socket, rd_buf, BUFSIZE));

    rd_buf[m_rd_bytes] = '\0';
    msgpack::unpack(m_unpacker, rd_buf, m_rd_bytes);
    m_unpacker.get().convert(*this);

}

template<typename T>
void SignalSocket::set(const std::string &key, const T &value, uint8_t data_type) {

    m_data[key].first = _to_string(value);
    m_data[key].second = data_type;

}

inline void SignalSocket::send() {

    m_packer.pack(*this);

    (m_server_side) ? write(m_rd_socket, m_sbuf.data(), m_sbuf.size())
                    : write(m_socket, m_sbuf.data(), m_sbuf.size());

    m_sbuf.clear();

}

#endif
