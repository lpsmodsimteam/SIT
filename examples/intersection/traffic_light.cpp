#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/elementinfo.h>

#define SIMTIME 100

class traffic_light : public SST::Component {

public:

    traffic_light(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    void handle_data_out(SST::Event *);

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
        { "stoplight_din", "Stoplight data_in", { "sst.Interfaces.StringEvent" }},
        { "stoplight_dout", "Stoplight data_out", { "sst.Interfaces.StringEvent" }},
        { "port", "Port on which to send/recv messages", { "sst.Interfaces.StringEvent" }}
    )

private:

    std::string m_clock;
    SST::Link *port, *m_din_link, *m_dout_link;
    SST::Output m_output;

    int GREENTIME;
    int YELLOWTIME;
    int REDTIME;
    int STARTGREEN;
    char light_state[2]{};
    char s[3]{};
};

traffic_light::traffic_light(SST::ComponentId_t id, SST::Params &params) :
    SST::Component(id),
    // Collect all the parameters from the project driver
    m_clock(params.find<std::string>("clock", "1Hz")),
    GREENTIME(params.find<int>("GREENTIME", 30)),
    YELLOWTIME(params.find<int>("YELLOWTIME", 3)),
    REDTIME(params.find<int>("REDTIME", 33)),
    STARTGREEN(params.find<int>("STARTGREEN", false)),
    port(configureLink("port")),
    m_din_link(configureLink("stoplight_din")),
    m_dout_link(configureLink(
        "stoplight_dout",
        new SST::Event::Handler<traffic_light>(this, &traffic_light::handle_data_out)
    )) {

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
    if (!(port && m_din_link && m_dout_link)) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure port 'port'\n");
    }

}

void traffic_light::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

}

void traffic_light::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());

}

void traffic_light::handle_data_out(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        m_output.verbose(CALL_INFO, 1, 0, "state -> %s\n", se->getString().c_str());
        strncpy(light_state, se->getString().c_str(), 1);
    }
    delete ev;

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

    m_din_link->send(new SST::Interfaces::StringEvent(
        std::to_string(keep_send) +
        std::to_string(keep_recv) +
        std::to_string(load) +
        std::to_string(start_green) +
        std::to_string(green_time) +
        std::to_string(yellow_time) +
        std::to_string(red_time) +
        std::to_string(current_cycle)));

    std::string c;
    switch (light_state[0]) {
        case '0':
            c = "green";
            break;
        case '1':
            c = "yellow";
            break;
        case '2':
            c = "red";
            break;
//        default:
//            c = "green";
    }

    port->send(new SST::Interfaces::StringEvent(c));
//    std::cout << "light_state " << c << std::endl;

    return false;

}
