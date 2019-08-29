#include "../../../ssti/ssti.hpp"

#include <sst/core/sst_config.h>
#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

class ram : public SST::Component {

public:

    ram(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t) { return false; };

    void handle_event(SST::Event *);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        ram, // class
        "tests", // element library
        "ram", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Demonstration of a PyRTL hardware simulation in SST",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "ram_din", "RAM data_in", { "sst.Interfaces.StringEvent" }},
        { "ram_dout", "RAM data_out", { "sst.Interfaces.StringEvent" }},
    )

private:

    // Prepare the signal handler
    SocketSignal m_signal_io;

    // SST parameters
    SST::Output m_output;
    std::string m_clock, m_proc, m_ipc_port;
    SST::Link *m_din_link, *m_dout_link;

};

ram::ram(SST::ComponentId_t id, SST::Params &params)
    : SST::Component(id),
      m_signal_io(0, socket(AF_UNIX, SOCK_STREAM, 0)),
      m_clock(params.find<std::string>("clock", "")),
      m_proc(params.find<std::string>("proc", "")),
      m_ipc_port(params.find<std::string>("ipc_port", "")),
      m_din_link(configureLink(
          "ram_din", new SST::Event::Handler<ram>(this, &ram::handle_event)
      )),
      m_dout_link(configureLink("ram_dout")) {

    m_output.init("\033[32mblackbox-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    registerClock(m_clock, new SST::Clock::Handler<ram>(this, &ram::tick));

    if (!(m_din_link && m_dout_link)) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure port\n");
    }

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

void ram::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    int child_pid = fork();

    if (!child_pid) {

        char *args[] = {(char *) "python3", &m_proc[0u], &m_ipc_port[0u], nullptr};
        m_output.verbose(CALL_INFO, 1, 0, "Forking process \"%s\"...\n", m_proc.c_str());
        execvp(args[0], args);

    } else {

        m_signal_io.set_addr(m_ipc_port);
        m_signal_io.recv();

        if (child_pid == m_signal_io.get()) {
            m_output.verbose(CALL_INFO, 1, 0, "Process \"%s\" successfully synchronized\n",
                             m_proc.c_str());
        }

    }

}

void ram::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());

}


void ram::handle_event(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se) {

        std::string _data_in = se->getString();
        bool keep_send = _data_in.substr(0, 1) != "0";
        bool keep_recv = _data_in.substr(1, 1) != "0";

        _data_in = 'X' + _data_in.substr(2);

        // inputs from parent SST model, outputs to PyRTL child process
        m_signal_io.set(_data_in);

        if (keep_send) {
            m_signal_io.set_state(keep_recv);
            m_signal_io.send();
        }
        if (keep_recv) {
            m_signal_io.recv();
        }

        // inputs to parent SST model, outputs from tests child process
        std::string _data_out = std::to_string(m_signal_io.get());
        m_dout_link->send(new SST::Interfaces::StringEvent(_data_out));

    }

    delete ev;

}
