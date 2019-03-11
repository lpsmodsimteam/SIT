#ifndef SIGUTILS_HPP
#define SIGUTILS_HPP

#include <msgpack.hpp>

#include <sstream>
#include <unistd.h>
#include <unordered_map>
#include <utility>      // std::pair, std::make_pair

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SC_BIT_T 0
#define SC_UINT_T 1
#define SC_STR_T 2

#define BUFSIZE 1024

template<typename T>
std::string _to_string(const T &);

template<typename T>
std::string _to_string(const T &value) {

    std::ostringstream ss;
    ss << value;
    return ss.str();

}

class SignalReceiver {

private:

    int m_socket, valread;
    struct sockaddr_un m_addr;
    char buffer[BUFSIZE];

    msgpack::unpacked m_unpacker;
    msgpack::packer<msgpack::sbuffer> m_packer;
    msgpack::sbuffer m_sbuf;
    std::unordered_map<std::string, std::pair<std::string, uint8_t >> m_data;

public:

    MSGPACK_DEFINE (m_data);

    explicit SignalReceiver(int, const std::string &);

    ~SignalReceiver();

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


inline SignalReceiver::SignalReceiver(int socket, const std::string &addr) :
    m_socket(socket), m_packer(&m_sbuf) {

    if (m_socket < 0) {
        printf("\n Socket creation error \n");
        exit(-1);
    }

    memset(&m_addr, '0', sizeof(m_addr));
    m_addr.sun_family = AF_UNIX;
    strcpy(m_addr.sun_path, addr.c_str());

    if (connect(m_socket, (struct sockaddr *) &m_addr, sizeof(m_addr)) < 0) {
        printf("\nConnection Failed \n");
        exit(-1);
    }

}

inline SignalReceiver::~SignalReceiver() {

    m_data.clear();

}

inline bool SignalReceiver::get_clock_pulse(const std::string &key) {

    return (this->get<int>(key)) % 2;

}

inline bool SignalReceiver::alive() {

    return (this->get<bool>("__on__"));

}

inline void SignalReceiver::set_state(bool state) {

    this->set("__on__", state);

}

template<typename T>
T SignalReceiver::get(const std::string &key) {

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

inline void SignalReceiver::recv() {

    valread = read(m_socket, buffer, BUFSIZE);
    buffer[valread] = '\0';
    msgpack::unpack(m_unpacker, (char *) buffer, valread);
    m_unpacker.get().convert(*this);

}

template<typename T>
void SignalReceiver::set(const std::string &key, const T &value, uint8_t data_type) {

    m_data[key].first = _to_string(value);
    m_data[key].second = data_type;

}

inline void SignalReceiver::send() {

    m_packer.pack(*this);
    write(m_socket, m_sbuf.data(), m_sbuf.size());
    m_sbuf.clear();

}

#endif
