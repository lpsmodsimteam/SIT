#include "../../src/sstscit.hpp"

#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/elementinfo.h>

#define SIMTIME 86400

class traffic_light : public SST::Component {

public:

    traffic_light(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    SST_ELI_REGISTER_COMPONENT(
        traffic_light,
        "intersection",
        "traffic_light",
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Demonstration of a PyRTL hardware simulation in SST",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "port", "Port on which to send/recv messages", { "sst.Interfaces.StringEvent" }}
    )

private:

    // Prepare the signal handler
    SocketSignal m_signal_io;

    // SST parameters
    SST::Link *port;
    std::string m_clock, m_proc, m_ipc_port;
    SST::Output m_output;
    int GREENTIME, YELLOWTIME, REDTIME, STARTGREEN;

    std::string light_state;
    char s[3]{};
};

traffic_light::traffic_light(SST::ComponentId_t id, SST::Params &params) :
    SST::Component(id),
    m_signal_io(socket(AF_UNIX, SOCK_STREAM, 0)),
    // Collect all the parameters from the project driver
    m_clock(params.find<std::string>("clock", "1Hz")),
    GREENTIME(params.find<int>("GREENTIME", 30)),
    YELLOWTIME(params.find<int>("YELLOWTIME", 3)),
    REDTIME(params.find<int>("REDTIME", 33)),
    STARTGREEN(params.find<int>("STARTGREEN", false)),
    m_proc(params.find<std::string>("proc", "")),
    m_ipc_port(params.find<std::string>("ipc_port", "")),
    port(configureLink("port")) {

    m_output.init("traffic_light-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

    // Check parameters
    if (!(GREENTIME && YELLOWTIME && REDTIME)) {
        m_output.fatal(CALL_INFO, -1, "Error: times must be greater than zero.\n");
    }

    // Print parameters
    m_output.verbose(CALL_INFO, 1, 0, "GREENTIME=%d, YELLOWTIME=%d, REDTIME=%d, STARTGREEN=%d\n",
                     GREENTIME, YELLOWTIME, REDTIME, STARTGREEN);

    // Just register a plain clock for this simple example
    registerClock(m_clock, new SST::Clock::Handler<traffic_light>(this, &traffic_light::tick));

    // Configure our ports
    if (!port) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure port 'port'\n");
    }

}

void traffic_light::setup() {

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

void traffic_light::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());

}

// Send a command to the PyRTL stopLight every clock
bool traffic_light::tick(SST::Cycle_t current_cycle) {

    bool keep_send = current_cycle < SIMTIME;
    bool keep_recv = current_cycle < SIMTIME - 1;

    bool load;
    bool start_green;
    int green_time;
    int yellow_time;
    int red_time;

    // turn reset off at 3 ns
    if (current_cycle == 1) {
        m_output.verbose(CALL_INFO, 1, 0, "INITIAL VALUES\n");
        load = true;
        start_green = STARTGREEN;
        green_time = GREENTIME;
        yellow_time = YELLOWTIME;
        red_time = REDTIME;
    } else {
        load = false;
        start_green = false;
        green_time = 0;
        yellow_time = 0;
        red_time = 0;
    }

    // inputs from parent SST model, outputs to SystemC child process
    m_signal_io.set("load", load);
    m_signal_io.set("start_green", start_green);
    m_signal_io.set("green_time", green_time);
    m_signal_io.set("yellow_time", yellow_time);
    m_signal_io.set("red_time", red_time);
    m_signal_io.set("clock", clock);

    if (keep_send) {
        m_signal_io.set_state(keep_recv);
        m_signal_io.send();
    }
    if (keep_recv) {
        m_signal_io.recv();
    }

    std::string c;
    switch (m_signal_io.get<int>("state")) {
        case 0:
            c = "green";
            break;
        case 1:
            c = "yellow";
            break;
        case 2:
            c = "red";
            break;
    }

    port->send(new SST::Interfaces::StringEvent(c));
//    std::cout << "light_state " << c << std::endl;

    return false;

}
