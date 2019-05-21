#include "galois_lfsr_ports.hpp"
#include "../../../sstscit/sstscit.hpp"

#include <sst/core/component.h>
#include <sst/core/elementinfo.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

class galois_lfsr : public SST::Component {

public:

    galois_lfsr(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t) { return false; };

    void handle_event(SST::Event *);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        galois_lfsr, // class
        "proto", // element library
        "galois_lfsr", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Simple 4-bit Galois Linear Feedback Shift Register",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "galois_lfsr_din", "Galois LFSR data_in", { "sst.Interfaces.StringEvent" }},
        { "galois_lfsr_dout", "Galois LFSR data_out", { "sst.Interfaces.StringEvent" }},
    )

private:

    // Prepare the signal handler
    zmq::context_t m_context;
    zmq::socket_t m_socket;
    ZMQReceiver m_signal_i;
    ZMQTransmitter m_signal_o;

    // SST parameters
    SST::Output m_output;
    std::string m_clock, m_proc, m_ipc_port;
    SST::Link *m_din_link, *m_dout_link;

};

galois_lfsr::galois_lfsr(SST::ComponentId_t id, SST::Params &params)
    : SST::Component(id),
      m_context(1), m_socket(m_context, ZMQ_REP),
      m_signal_i(GLSLFSR_NPORTS, m_socket), m_signal_o(GLSLFSR_NPORTS, m_socket),
      m_clock(params.find<std::string>("clock", "")),
      m_proc(params.find<std::string>("proc", "")),
      m_ipc_port(params.find<std::string>("ipc_port", "")),
      m_din_link(configureLink(
          "galois_lfsr_din", new SST::Event::Handler<galois_lfsr>(this, &galois_lfsr::handle_event)
      )),
      m_dout_link(configureLink("galois_lfsr_dout")) {

    m_output.init("\033[32mblackbox-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    registerClock(m_clock, new SST::Clock::Handler<galois_lfsr>(this, &galois_lfsr::tick));

    if (!(m_din_link && m_dout_link)) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure port\n");
    }

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

void galois_lfsr::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    int child_pid = fork();

    if (!child_pid) {

        char *args[] = {&m_proc[0u], &m_ipc_port[0u], nullptr};
        m_output.verbose(CALL_INFO, 1, 0, "Forking process \"%s\"...\n", m_proc.c_str());
        execvp(args[0], args);

    } else {

        m_socket.bind(m_ipc_port.c_str());
        m_signal_i.recv();
        if (child_pid == m_signal_i.get<int>(glslfsr_ports.pid)) {
            m_output.verbose(CALL_INFO, 1, 0, "Process \"%s\" successfully synchronized\n",
                             m_proc.c_str());
        }

    }

}

void galois_lfsr::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());
    m_socket.close();

}

void galois_lfsr::handle_event(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se) {

        std::string _data_in = se->getString();
        bool keep_send = _data_in.substr(0, 1) != "0";
        bool keep_recv = _data_in.substr(1, 1) != "0";

        // inputs from parent SST model, outputs to SystemC child process
        m_signal_o.set(glslfsr_ports.reset, std::stoi(_data_in.substr(2, 1)));
        m_signal_o.set(glslfsr_ports.clock, std::stoi(_data_in.substr(3)));

        if (keep_send) {
            m_signal_o.set_state(keep_recv);
            m_signal_o.send();
        }
        if (keep_recv) {
            m_signal_i.recv();
        }

        // inputs to parent SST model, outputs from SystemC child process
        std::string _data_out = std::to_string(m_signal_i.get<int>(glslfsr_ports.data_out));
        m_dout_link->send(new SST::Interfaces::StringEvent(_data_out));

    }

    delete ev;

}
