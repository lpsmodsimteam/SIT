/*
 * SignalIO abstract class definitions and implementations.
 */

#ifndef SIGUTILS_HPP
#define SIGUTILS_HPP

#include "msgpack/msgpack.hpp"  // to pack/unpack signals

#include <sstream>
#include <unistd.h>
#include <iostream>
#include <vector>

/*
 * Implements common methods for setting and getting data being transferred
 * via the preferred IPC.
 * 
 * This class is implicitly-abstract and needs to be inherited by a class that
 * implements a supported interprocess communication (IPC) protocol. No
 * constructors are necessary for this class.
*/
class SignalIO {

protected:

    explicit SignalIO(int);

    /*
     * Destructor - clears the member variable containing the transported
     * data.
     */
    ~SignalIO();

    /*
     * Unordered map containing the transported data. The keys and values are
     * both std::string.
     */
//    std::unordered_map<std::string, std::string> m_data;
    std::vector<std::string> m_data;

public:

    /*
     * Adds a new key-value pair to the member-variable `m_data`. `value`
     * is templated and therefore do not need to be restricted to a specific
     * type. `value` is streamed into a string stream to be stored in `m_data`
     * as a std::string.
     */
    template<typename T>
    void set(int, const T &);

    /*
     * Returns the value specified by `key`. The values are casted statically
     * as a templated type. The method raises a std::invalid_argument exception
     * if `key` does not exist in `m_data`.
     */
    template<typename T>
    T get(int);

    /*
     * Sets the reserved key `__on__` in `m_data` to the specified boolean
     * state to work alongside `alive()`. The reserved key itself has no
     * special usage besides providing a high-level access for the user to
     * determine the state of the child SystemC driver process.
     *
     * Example usage:
     *      `signal_io.set_state(true)` (equivalent to
     *      `signal_io.set("__on__", true)`) enables the child driver process
     *      to continue when it evaluates `alive()` on its thread.
     *
     *      `signal_io.set_state(false)` (equivalent to
     *      `signal_io.set("__on__", false)`) alerts the child driver process
     *      to call all its destructors to exit gracefully before the parent
     *      process kills it.
     */
    void set_state(bool);

    /*
     * Returns the reserved key `__on__` in `m_data`. The reserved key itself
     * has no special usage besides providing a high-level access for the user
     * to determine the state of the child SystemC driver process.
     *
     * Example usage:
     *      `signal_io.alive()` (equivalent to `signal_io.get<bool>("__on__")`)
     *      returns true when the parent process would like the child driver
     *      to continue to stay alive and communicate their signals. If the
     *      method returns false, the parent is done communicating with the
     *      child process and allows it to exit gracefully.
     */
    bool alive();

    /*
     * Converts SST clock cycles to pulses for SystemC modules.
     */
    bool get_clock_pulse(int);

};


/* -------------------- SIGNALIO IMPLEMENTATIONS -------------------- */

inline SignalIO::SignalIO(const int num_ports) {

    m_data.resize(num_ports);

}

inline SignalIO::~SignalIO() {

    m_data.clear();

}

template<typename T>
void SignalIO::set(const int key, const T &value) {

    std::ostringstream ss;
    ss << value;
    m_data[key] = ss.str();

}

template<typename T>
T SignalIO::get(const int key) {

    return static_cast<T>(std::stoi(m_data[key]));

}

inline bool SignalIO::alive() {

    return (this->get<bool>(0));

}

inline void SignalIO::set_state(bool state) {

    this->set(0, state);

}

inline bool SignalIO::get_clock_pulse(const int key) {

    return (this->get<int>(key)) % 2;

}

#endif  // SIGUTILS_HPP
