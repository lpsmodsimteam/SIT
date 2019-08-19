#include "../../blackboxes/traffic_light_fsm_ports.hpp"
#include "../../../../ssti/ssti.hpp"

#include <sst/core/sst_config.h>
#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#define SIMTIME 86400

class traffic_light_pyrtl : public SST::Component {

public:

    traffic_light_pyrtl(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    void send();

    void recv();

    void set_state(bool);

    SST_ELI_REGISTER_COMPONENT(
        traffic_light_pyrtl,
        "intersection",
        "traffic_light_pyrtl",
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Traffic light simulator for the intersection",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        { "CLOCK", "Clock frequency or period", "1Hz" },
        { "GREENTIME", "Duration of the green light staying on", "30" },
        { "YELLOWTIME", "Duration of the yellow light staying on", "3" },
        { "REDTIME", "Duration of the red light staying on", "33" },
        { "STARTGREEN", "Flag to initiate the light as green or red", "0" },
        { "PROC", "Path to compiled SystemC driver", "/path/to/systemc/driver" },
        { "IPC_PORT", "Path to the IPC file", "/tmp/ABCDEF" },
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "light_state", "Port on which to send/recv messages", { "sst.Interfaces.StringEvent" }}
    )

private:

    int m_socket;
    size_t m_rd_bytes;
    char m_buf[BUFSIZE];
    struct sockaddr_un m_addr;

    std::string m_data;

    // SST parameters
    std::string m_clock;
    int STARTGREEN, GREENTIME, YELLOWTIME, REDTIME;
    std::string m_proc, m_ipc_port;

    // SST links and variables
    SST::Output m_output;
    SST::Link *light_state;

};

traffic_light_pyrtl::traffic_light_pyrtl(SST::ComponentId_t id, SST::Params &params) :
    SST::Component(id),
    m_socket(socket(AF_UNIX, SOCK_STREAM, 0)),
    m_rd_bytes(0), m_buf(""), m_addr({}),
    // Collect all the parameters from the project driver
    m_clock(params.find<std::string>("CLOCK", "1Hz")),
    STARTGREEN(params.find<int>("STARTGREEN", false)),
    GREENTIME(params.find<int>("GREENTIME", 30)),
    YELLOWTIME(params.find<int>("YELLOWTIME", 3)),
    REDTIME(params.find<int>("REDTIME", 33)),
    m_proc(params.find<std::string>("PROC", "")),
    m_ipc_port(params.find<std::string>("IPC_PORT", "")),
    light_state(configureLink("light_state")) {

    m_output.init("\033[93mtraffic_light-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    // Check parameters
    if (!(GREENTIME && YELLOWTIME && REDTIME)) {
        m_output.fatal(CALL_INFO, -1, "Error: times must be greater than zero.\n");
    }

    // Print parameters
    m_output.verbose(CALL_INFO, 1, 0, "GREENTIME=%d, YELLOWTIME=%d, REDTIME=%d, STARTGREEN=%d\n",
                     GREENTIME, YELLOWTIME, REDTIME, STARTGREEN);

    // Just register a plain clock for this simple example
    registerClock(m_clock, new SST::Clock::Handler<traffic_light_pyrtl>(this, &traffic_light_pyrtl::tick));

    // Configure our ports
    if (!light_state) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure light_state 'light_state'\n");
    }

}

void traffic_light_pyrtl::recv() {

    m_rd_bytes = static_cast<size_t>(read(m_socket, m_buf, BUFSIZE));

    m_buf[m_rd_bytes] = '\0';
    m_data = std::string(m_buf);

}

void traffic_light_pyrtl::send() {

    write(m_socket, m_data.c_str(), m_data.size());

}

void traffic_light_pyrtl::set_state(bool state) {

    m_data[0] = state ? '1' : '0';

}


void traffic_light_pyrtl::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    int child_pid = fork();

    if (!child_pid) {

        char *args[] = {(char *) "python3", &m_proc[0u], &m_ipc_port[0u], nullptr};
        m_output.verbose(CALL_INFO, 1, 0, "Forking process \"%s\"...\n", m_proc.c_str());
        execvp(args[0], args);

    } else {

        if (m_socket < 0) {
            perror("Socket creation\n");
        }

        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sun_family = AF_UNIX;
        strcpy(m_addr.sun_path, m_ipc_port.c_str());


        if (bind(m_socket, (struct sockaddr *) &m_addr, sizeof(m_addr)) < 0) {
            perror("Bind failed\n");
        }

        if (listen(m_socket, 5) < 0) {
            perror("Socket listen\n");
        }

        socklen_t addr_len = sizeof(m_addr);
        if ((m_socket = accept(m_socket, (struct sockaddr *) &m_addr, &addr_len)) < 0) {
            perror("Accept failed\n");
        }

        this->recv();

        if (child_pid == std::stoi(m_data)) {
            m_output.verbose(CALL_INFO, 1, 0, "Process \"%s\" successfully synchronized\n",
                             m_proc.c_str());
        }

    }

}

void traffic_light_pyrtl::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());
    unlink(m_addr.sun_path);
    close(m_socket);

}

// Send a command to the PyRTL stopLight every clock
bool traffic_light_pyrtl::tick(SST::Cycle_t current_cycle) {

    bool keep_send = current_cycle < SIMTIME;
    bool keep_recv = current_cycle < SIMTIME - 1;

    if (keep_send) {

        char buff[10];
        if (current_cycle == 1) {
        	snprintf(buff, sizeof(buff), "X1%d%02d%02d%02d", STARTGREEN, GREENTIME, YELLOWTIME, REDTIME);
        } else {
        	strncpy(buff, "X00000000", sizeof(buff) - 1);
        }
        m_data = buff;

        this->set_state(keep_recv);
        this->send();
    }

    if (keep_recv) {
        this->recv();
    }

    switch (std::stoi(m_data)) {
        case 0:
            light_state->send(new SST::Interfaces::StringEvent("green"));
            break;
        case 1:
            light_state->send(new SST::Interfaces::StringEvent("yellow"));
            break;
        case 2:
            light_state->send(new SST::Interfaces::StringEvent("red"));
            break;
    }

    return false;

}
