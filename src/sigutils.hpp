#ifndef SIGUTILS_HPP
#define SIGUTILS_HPP

#include <msgpack.hpp>

#include <sstream>
#include <unistd.h>
#include <unordered_map>


class SignalIO {

protected:

    ~SignalIO();

    template<typename T>
    std::string _to_string(const T &);

    std::unordered_map<std::string, std::string> m_data;

public:

    template<typename T>
    void set(const std::string &, const T &);

    template<typename T>
    T get(const std::string &);

    void set_state(bool);

    bool alive();

    // Converts SST clock cycles to pulses for SystemC modules
    bool get_clock_pulse(const std::string &);

};


/* -------------------- SIGNALIO IMPLEMENTATIONS -------------------- */

inline SignalIO::~SignalIO() {

    m_data.clear();

}

template<typename T>
void SignalIO::set(const std::string &key, const T &value) {

    m_data[key] = _to_string(value);

}

template<typename T>
T SignalIO::get(const std::string &key) {

    if (m_data.find(key) != m_data.end()) {
        return std::stoi(m_data[key]);
    }

    throw std::invalid_argument("Key does not exist");

}

inline bool SignalIO::alive() {

    return (this->get<bool>("__on__"));

}

inline void SignalIO::set_state(bool state) {

    this->set("__on__", state);

}

inline bool SignalIO::get_clock_pulse(const std::string &key) {

    return (this->get<int>(key)) % 2;

}

template<typename T>
std::string SignalIO::_to_string(const T &value) {

    std::ostringstream ss;
    ss << value;
    return ss.str();

}

#endif  // SIGUTILS_HPP
