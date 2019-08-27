#include "{module_dir}{module}.hpp"
#include "{ports_dir}{module}_ports.hpp"
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
    {sender}.set({abbr}_ports.pid, getpid());
    {sender}.send();
    // ---------- INITIAL HANDSHAKE ---------- //

    while (true) {{

        sc_start();

        // RECEIVING
        {receiver}.recv();

        if (!{receiver}.alive()) {{
            break;
        }}
        {clock};
        {inputs};

        // SENDING
        {outputs};
        {sender}.send();

    }}

    {dest}

    return 0;

}}
