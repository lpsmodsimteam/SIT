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
    {var_decl}
    // ---------- IPC SOCKET SETUP AND HANDSHAKE ---------- //

    // ---------- INITIAL HANDSHAKE ---------- //
    {sender}.set(std::to_string(getpid()));
    {sender}.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    std::ostringstream ss;

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
        std::string _data_out = ss.str();

        {sender}.set(_data_out);
        {sender}.send();

        ss.str(std::string());


    }}

    {dest}

    return 0;

}}
