#ifndef SIGUTILS_HPP
#define SIGUTILS_HPP

#include <msgpack.hpp>
#include <zmq.hpp>

#include <sstream>
#include <unistd.h>
#include <unordered_map>
#include <utility>      // std::pair, std::make_pair

#define SC_BIT_T 1
#define SC_UINT_T 2

template<typename T>
std::string _to_string(const T &);

class SignalReceiver {

private:

    zmq::socket_t &m_socket;
    zmq::message_t m_buf;
    msgpack::unpacked m_unpacker;
    std::unordered_map<std::string, std::pair<std::string, uint8_t >> m_data;


public:

    MSGPACK_DEFINE (m_data);

    explicit SignalReceiver(zmq::socket_t &);

    ~SignalReceiver();

    template<typename T>
    T get(const std::string &);

    void recv();

    bool alive();

    // Converts SST clock cycles to pulses for SystemC modules
    bool get_clock_pulse(const std::string &);

};

class SignalTransmitter {

private:

    zmq::socket_t &m_socket;
    zmq::message_t m_buf;
    msgpack::packer<msgpack::sbuffer> m_packer;
    msgpack::sbuffer m_sbuf;
    std::unordered_map<std::string, std::pair<std::string, uint8_t >> m_data;

public:

    MSGPACK_DEFINE (m_data);

    explicit SignalTransmitter(zmq::socket_t &);

    ~SignalTransmitter();

    void set_state(bool);

    template<typename T>
    void set(const std::string &, const T &, uint8_t = SC_BIT_T);

    void send();

};


/* -------------------- IMPLEMENTATIONS -------------------- */


inline SignalReceiver::SignalReceiver(zmq::socket_t &socket) :
    m_socket(socket) {
    // do nothing
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

inline void SignalTransmitter::set_state(bool state) {

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

    m_socket.recv(&m_buf);
    msgpack::unpack(m_unpacker, (char *) (m_buf.data()), m_buf.size());
    m_unpacker.get().convert(*this);

}


inline SignalTransmitter::SignalTransmitter(zmq::socket_t &socket) :
    m_socket(socket), m_packer(&m_sbuf) {
    // do nothing
}

inline SignalTransmitter::~SignalTransmitter() {

    m_data.clear();
    m_sbuf.clear();

}


template<typename T>
std::string _to_string(const T &value) {

    std::ostringstream ss;
    ss << value;
    return ss.str();

}


template<typename T>
void SignalTransmitter::set(const std::string &key, const T &value, uint8_t data_type) {

    m_data[key].first = _to_string(value);
    m_data[key].second = data_type;

}


inline void SignalTransmitter::send() {

    m_packer.pack(*this);
    m_buf.rebuild(m_sbuf.size());
    std::memcpy(m_buf.data(), m_sbuf.data(), m_sbuf.size());
    m_socket.send(m_buf);
    m_sbuf.clear();

}

#endif
