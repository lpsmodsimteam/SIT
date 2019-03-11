#ifndef SIGUTILS_HPP
#define SIGUTILS_HPP

#include <msgpack.hpp>

#include <sstream>
#include <unistd.h>
#include <unordered_map>
#include <utility>      // std::pair, std::make_pair

#define SC_BIT_T 0
#define SC_UINT_T 1
#define SC_STR_T 2


class SignalReceiver {

protected:

    std::unordered_map<std::string, std::pair<std::string, uint8_t >> m_data;

public:

    template<typename T>
    T get(const std::string &);

    bool alive();

    // Converts SST clock cycles to pulses for SystemC modules
    bool get_clock_pulse(const std::string &);

};

class SignalTransmitter {

protected:

    std::unordered_map<std::string, std::pair<std::string, uint8_t >> m_data;

    template<typename T>
    std::string _to_string(const T &);

public:

    void set_state(bool);

    template<typename T>
    void set(const std::string &, const T &, uint8_t = SC_BIT_T);

};


/* -------------------- SIGNALRECEIVER IMPLEMENTATIONS -------------------- */


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

template<typename T>
void SignalTransmitter::set(const std::string &key, const T &value, uint8_t data_type) {

    m_data[key].first = _to_string(value);
    m_data[key].second = data_type;

}

template<typename T>
std::string SignalTransmitter::_to_string(const T &value) {

    std::ostringstream ss;
    ss << value;
    return ss.str();

}

#endif
