/*
 * SITBuffer abstract class definitions and implementations.
 */

#ifndef SIGUTILS
#define SIGUTILS

#include <unistd.h>

#include <iomanip>
#include <sstream>

/*
 * Implements methods for setting and getting data being transferred via the
 * preferred IPC.
 *
 * This class is implicitly-abstract and needs to be inherited by a class that
 * implements a supported interprocess communication (IPC) protocol.
 */
class SITBuffer {
   protected:
    explicit SITBuffer(int);

    ~SITBuffer();

    int m_buf_size;
    char* m_data;

   public:
    /* FINAL METHODS - CANNOT BE OVERRIDDEN BY CHILD CLASS */
    virtual void set(const std::string&) final;

    virtual std::string get() final;

    virtual void set_state(bool) final;

    /* PURE VIRTUAL METHODS - MUST BE IMPLEMENTED BY CHILD CLASS */
    virtual void recv() = 0;

    virtual void send() = 0;

    /* OVERLOADED METHODS - EXACTLY ONE MUST BE IMPLEMENTED BY CHILD CLASS */
    void set_addr(const std::string&);

    void set_addr(const std::string&, const std::string&);

    /*
    Align buffer widths by left-padding with place-holding '0'
    */
    static void align_buffer_width(int width, std::string& buffer);

    static void
    align_buffer_width(int width, float buffer, std::string& new_buffer);

    static void append_buffer(const char chr, int width, std::string& buffer);
};

/* -------------------- SIGNALIO IMPLEMENTATIONS -------------------- */

/*
 * Allocate the transported data on heap
 */
inline SITBuffer::SITBuffer(int buf_size)
    : m_buf_size(buf_size), m_data(new char[m_buf_size]) {
    // do nothing
}

/*
 * Clears the member variable containing the transported data
 */
inline SITBuffer::~SITBuffer() {
    delete[] m_data;
}

/*
 * Assigns a new value to the member-variable std::string `m_data`.
 *
 * Arguments:
 *     values -- values of the transport data
 */
inline void SITBuffer::set(const std::string& values) {
    snprintf(m_data, values.size() + 1, "%s", values.c_str());
}

/*
 * Returns the value specified by `index`. The values are casted statically as a
 * templated type.
 */
inline std::string SITBuffer::get() {
    return m_data;
}

/*
 * Sets the reserved index 0 in `m_data` to the specified boolean state
 *
 * The reserved index itself has no special usage besides providing a high-level
 * access for the user to determine the state of the child SystemC driver
 * process. The reserved index 0 is initially used during the handshaking phase
 * to check the PID of the child process for synchronization and is no longer
 * necessary afterwards. Using the method before or during the handshaking phase
 * will cause the parent process to evaluate its child process's PID
 * incorrectly.
 *
 * Example usages (post-handshake phases):
 *      * `signal_io.set_state(true)` (equivalent to `m_data[0] = '1'`) enables
 * the child driver process to continue when it evaluates `signal_io.get()[0]`
 * on its thread.
 *
 *      * `signal_io.set_state(false)` (equivalent to `m_data[0] = '0'`) alerts
 * the child driver process to call all its destructors to exit gracefully
 * before the parent process kills it.
 */
inline void SITBuffer::set_state(bool state) {
    m_data[0] = state ? '1' : '0';
}

/*
Align buffer widths by left-padding with place-holding '0'
*/
void SITBuffer::align_buffer_width(int width, std::string& buffer) {
    int _len = buffer.length();
    if (_len < width) {
        buffer = std::string(width - _len, '0') + buffer;
    }
}

void SITBuffer::align_buffer_width(
    int width,
    float buffer,
    std::string& new_buffer
) {
    std::ostringstream _data_out;
    _data_out << std::fixed << std::setprecision(width) << buffer;
    new_buffer = _data_out.str().substr(0, width);
}

void SITBuffer::append_buffer(const char chr, int width, std::string& buffer) {
    int _len = buffer.length();
    if (_len < width) {
        buffer += std::string(width - _len, chr);
    }
}

#endif  // SIGUTILS
