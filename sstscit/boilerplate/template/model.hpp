#include "{module}_ports.hpp"
#include "sstscit.hpp"

#include <sst/core/component.h>
#include <sst/core/elementinfo.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

class {module} : public SST::Component {{

public:

    {module}(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t) {{ return false; }};

    void handle_event(SST::Event *);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        {module}, // class
        "{lib}", // element library
        "{comp}", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "{desc}",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        {{ "{module}_din", "{link_desc0}", {{ "sst.Interfaces.StringEvent" }}}},
        {{ "{module}_dout", "{link_desc1}", {{ "sst.Interfaces.StringEvent" }}}},
    )

private:

    // Prepare the signal handler
    {var_decl}

    // SST parameters
    SST::Output m_output;
    std::string m_clock, m_proc, m_ipc_port;
    SST::Link *m_din_link, *m_dout_link;

}};

{module}::{module}(SST::ComponentId_t id, SST::Params &params)
    : SST::Component(id),
      {var_init}
      m_clock(params.find<std::string>("clock", "")),
      m_proc(params.find<std::string>("proc", "")),
      m_ipc_port(params.find<std::string>("ipc_port", "")),
      m_din_link(configureLink(
          "{module}_din", new SST::Event::Handler<{module}>(this, &{module}::handle_event)
      )),
      m_dout_link(configureLink("{module}_dout")) {{

    m_output.init("\033[32mblackbox-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    registerClock(m_clock, new SST::Clock::Handler<{module}>(this, &{module}::tick));

    if (!(m_din_link && m_dout_link)) {{
        m_output.fatal(CALL_INFO, -1, "Failed to configure port\n");
    }}

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}}

void {module}::setup() {{

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    int child_pid = fork();

    if (!child_pid) {{

        char *args[] = {{&m_proc[0u], &m_ipc_port[0u], nullptr}};
        m_output.verbose(CALL_INFO, 1, 0, "Forking process \"%s\"...\n", m_proc.c_str());
        execvp(args[0], args);

    }} else {{

        {var_bind};
        {receiver}.recv();
        if (child_pid == {receiver}.get<int>({module}_ports::__pid__)) {{
            m_output.verbose(CALL_INFO, 1, 0, "Process \"%s\" successfully synchronized\n",
                             m_proc.c_str());
        }}

    }}

}}

void {module}::finish() {{

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());
    {var_dest}

}}

void {module}::handle_event(SST::Event *ev) {{

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se) {{

        std::string _data_in = se->getString();
        bool keep_send = _data_in.substr(0, 1) != "0";
        bool keep_recv = _data_in.substr(1, 1) != "0";

        // inputs from parent SST model, outputs to SystemC child process
        {outputs};

        if (keep_send) {{
            {sender}.set_state(keep_recv);
            {sender}.send();
        }}
        if (keep_recv) {{
            {receiver}.recv();
        }}

        // inputs to parent SST model, outputs from SystemC child process
        std::string _data_out = {inputs};
        m_dout_link->send(new SST::Interfaces::StringEvent(_data_out));

    }}

    delete ev;

}}
