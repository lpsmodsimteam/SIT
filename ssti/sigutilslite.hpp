/*
 * SignalIO abstract class definitions and implementations.
 */

#ifndef SIGUTILSLITE_HPP
#define SIGUTILSLITE_HPP

#include <string>
#include <sstream>
#include <unistd.h>

/*
 * Implements common methods for setting and getting data being transferred via the preferred IPC.
 * 
 * This class is implicitly-abstract and needs to be inherited by a class that implements a
 * supported interprocess communication (IPC) protocol.
*/
class SignalIO {

protected:

    explicit SignalIO(int);

    ~SignalIO();

    // vector of std::string containing the transported data
    std::string m_data;

public:

    template<typename... T>
    void set(char, const T &...);

    template<typename T>
    T get(int);

    void set_state(bool);

    bool alive();

};


/* -------------------- SIGNALIO IMPLEMENTATIONS -------------------- */

/*
 * Sets the size of the member std::vector
 *
 * Arguments:
 *     num_ports -- Number of ports used in the black box interface. This number is usually 1
 *                  greater than the total number of the SystemC module ports
 */
inline SignalIO::SignalIO(const int num_ports) {

    // m_data.resize(num_ports);

}

/*
 * Clears the member variable containing the transported data
 */
inline SignalIO::~SignalIO() {

    m_data.clear();

}

/*
 * Assigns a new value to the member-variable std::vector `m_data`.
 *
 * Arguments:
 *     index -- index of `m_data` for the new value to be set. The value of this index is hardcoded
 *              in the ports struct data structure in the black box interface.
 *     value -- value of the port being stored. This argument is templated and therefore do not
 *              need to be restricted to a specific type. `value` is streamed into a string stream
 *              to be stored in `m_data` as a std::string.
 */
template<typename... T>
void SignalIO::set(const char fmt, const T &... values) {

    snprintf(m_data, m_data.size(), fmt, values...);

}

/*
 * Returns the value specified by `index`. The values are casted statically as a templated type.
 */
template<typename T>
T SignalIO::get(const int index) {

    return static_cast<T>(std::stoi(m_data));

}

/*
 * Returns the reserved index 0 in `m_data`. The reserved index itself has no special usage besides
 * providing a high-level access for the user to determine the state of the child SystemC driver
 * process.
 *
 * Example usage:
 *      `signal_io.alive()` (equivalent to `signal_io.get<bool>(0)`) returns true when the parent
 *      process would like the child driver to continue to stay alive and communicate their
 *      signals. If the method returns false, the parent is done communicating with the child
 *      process and allows it to exit gracefully.
 */
inline bool SignalIO::alive() {

    return m_data[0];

}

/*
 * Sets the reserved index 0 in `m_data` to the specified boolean state to work alongside
 * `alive()`.
 *
 * The reserved index itself has no special usage besides providing a high-level access for the
 * user to determine the state of the child SystemC driver process. The reserved index 0 is
 * initially used during the handshaking phase to check the PID of the child process for
 * synchronization and is no longer necessary afterwards. Using the method before or during the
 * handshaking phase will cause the parent process to evaluate its child process's PID incorrectly.
 *
 * Example usages (post-handshake phases):
 *      * `signal_io.set_state(true)` (equivalent to `signal_io.set(0, true)`) enables the child
 *      driver process to continue when it evaluates `alive()` on its thread.
 *
 *      * `signal_io.set_state(false)` (equivalent to `signal_io.set(0, false)`) alerts the child
 *      driver process to call all its destructors to exit gracefully before the parent process
 *      kills it.
 */
inline void SignalIO::set_state(bool state) {

    m_data[0] = state ? '1' : '0';

}

#endif  // SIGUTILSLITE_HPP
