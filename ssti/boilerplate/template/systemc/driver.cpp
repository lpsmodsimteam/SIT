#include "{module_dir}{module}.hpp"
#include "{lib_dir}ssti.hpp"

int sc_main(int, char *argv[]) {{

    // ---------- SYSTEMC UUT INIT ---------- //
    {port_defs};

    // Connect the DUT
    {module} DUT("{module}");
    {bindings};
    // ---------- SYSTEMC UUT INIT ---------- //

    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //
    // Initialize signal handlers
    {sig_type} m_signal_io({buf_size}, false);
    m_signal_io.set_addr(argv[1]);
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    {sender}.set(std::to_string(getpid()));
    {sender}.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    std::ostringstream _data_out;

    while (true) {{

        // RECEIVING
        {receiver}.recv();
        std::string _data_in = {receiver}.get();

        if (_data_in[0] == '0') {{
            break;
        }}
        {inputs}

        // SENDING
        sc_start();

        {outputs};

        {sender}.set(_data_out.str());
        {sender}.send();

        _data_out.str(std::string());

    }}

    return 0;

}}
