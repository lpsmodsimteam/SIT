#include "../../../src/sstscit.hpp"

#include <sst/core/component.h>
#include <sst/core/elementinfo.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

class stoplight : public SST::Component {

public:

    stoplight(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t) { return false; };

    void handle_event(SST::Event *);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        stoplight, // class
        "intersection", // element library
        "stoplight", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Demonstration of a SystemC hardware simulation in SST",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "stoplight_din", "Stoplight data_in", { "sst.Interfaces.StringEvent" }},
        { "stoplight_dout", "Stoplight data_out", { "sst.Interfaces.StringEvent" }},
    )

private:

    // Prepare the signal handler
    SocketSignal m_signal_io;

    // SST parameters
    SST::Output m_output;
    std::string m_clock, m_proc, m_ipc_port;
    SST::Link *m_din_link, *m_dout_link;

};

stoplight::stoplight(SST::ComponentId_t id, SST::Params &params)
    : SST::Component(id),
      m_signal_io(socket(AF_UNIX, SOCK_STREAM, 0)),
      m_clock(params.find<std::string>("clock", "")),
      m_proc(params.find<std::string>("proc", "")),
      m_ipc_port(params.find<std::string>("ipc_port", "")),
      m_din_link(configureLink(
          "stoplight_din", new SST::Event::Handler<stoplight>(this, &stoplight::handle_event)
      )),
      m_dout_link(configureLink("stoplight_dout")) {

    m_output.init("\033[32mblackbox-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    registerClock(m_clock, new SST::Clock::Handler<stoplight>(this, &stoplight::tick));

    if (!(m_din_link && m_dout_link)) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure port\n");
    }

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

void stoplight::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    int child_pid = fork();

    if (!child_pid) {

        char *args[] = {&m_proc[0u], &m_ipc_port[0u], nullptr};
        m_output.verbose(CALL_INFO, 1, 0, "Forking process \"%s\"...\n", m_proc.c_str());
        execvp(args[0], args);

    } else {

        m_signal_io.set_addr(m_ipc_port);
        m_signal_io.recv();
        if (child_pid == m_signal_io.get<int>("__pid__")) {
            m_output.verbose(CALL_INFO, 1, 0, "Process \"%s\" successfully synchronized\n",
                             m_proc.c_str());
        }

    }

}

void stoplight::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());
    

}

void stoplight::handle_event(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se) {

        std::string _data_in = se->getString();
        bool keep_send = _data_in.substr(0, 1) != "0";
        bool keep_recv = _data_in.substr(1, 1) != "0";

        // inputs from parent SST model, outputs to SystemC child process
        m_signal_io.set("load", std::stoi(_data_in.substr(2, 1)));
        m_signal_io.set("start_green", std::stoi(_data_in.substr(3, 1)));
        m_signal_io.set("green_time", std::stoi(_data_in.substr(4, 2)));
        m_signal_io.set("yellow_time", std::stoi(_data_in.substr(6, 1)));
        m_signal_io.set("red_time", std::stoi(_data_in.substr(7, 2)));
        m_signal_io.set("clock", std::stoi(_data_in.substr(9, 2)));

        if (keep_send) {
            m_signal_io.set_state(keep_recv);
            m_signal_io.send();
        }
        if (keep_recv) {
            m_signal_io.recv();
        }

        // inputs to parent SST model, outputs from SystemC child process
        std::string _data_out = std::to_string(m_signal_io.get<int>("state"));
        m_dout_link->send(new SST::Interfaces::StringEvent(_data_out));

    }

    delete ev;

}
