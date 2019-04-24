#include "{module}.hpp"

#include "sstscit.hpp"

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
    {sender}.set("__pid__", getpid());
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
