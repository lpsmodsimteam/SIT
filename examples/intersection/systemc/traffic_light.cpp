#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#define SIMTIME 86400

class traffic_light : public SST::Component {

public:

    traffic_light(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    void handle_event(SST::Event *);

    SST_ELI_REGISTER_COMPONENT(
        traffic_light,
        "intersection",
        "traffic_light",
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Traffic light simulator for the intersection",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        { "GREENTIME", "Duration of the green light staying on", "30" },
        { "YELLOWTIME", "Duration of the yellow light staying on", "3" },
        { "REDTIME", "Duration of the red light staying on", "33" },
        { "STARTGREEN", "Flag to initiate the light as green or red", "0" },
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "sc_din", "Traffic Light FSM data_in", { "sst.Interfaces.StringEvent" }},
        { "sc_dout", "Traffic Light FSM data_out", { "sst.Interfaces.StringEvent" }},
        { "light_state", "Port on which to send/recv messages", { "sst.Interfaces.StringEvent" }},
    )

private:

    // SST parameters
    std::string m_clock;
    int STARTGREEN, GREENTIME, YELLOWTIME, REDTIME;

    // SST links and variables
    SST::Output m_output;
    SST::Link *sc_din_link, *sc_dout_link, *light_state;

    unsigned int m_cycle{};

};

traffic_light::traffic_light(SST::ComponentId_t id, SST::Params &params) :
    SST::Component(id),
    // Collect all the parameters from the project driver
    m_clock(params.find<std::string>("CLOCK", "1Hz")),
    STARTGREEN(params.find<int>("STARTGREEN", false)),
    GREENTIME(params.find<int>("GREENTIME", 30)),
    YELLOWTIME(params.find<int>("YELLOWTIME", 3)),
    REDTIME(params.find<int>("REDTIME", 33)),
    sc_din_link(configureLink("sc_din")),
    sc_dout_link(configureLink(
        "sc_dout",
        new SST::Event::Handler<traffic_light>(this, &traffic_light::handle_event))),
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
    registerClock(m_clock, new SST::Clock::Handler<traffic_light>(this, &traffic_light::tick));

    // Configure our ports
    if (!light_state) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure light_state 'light_state'\n");
    }

}

void traffic_light::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

}

void traffic_light::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());

}

void traffic_light::handle_event(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {

        if (m_cycle < SIMTIME - 2) {

            switch (std::stoi(se->getString())) {
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
        }
    }

}


// Send a command to the PyRTL stopLight every clock
bool traffic_light::tick(SST::Cycle_t current_cycle) {

    bool keep_send = current_cycle < SIMTIME;
    bool keep_recv = current_cycle < SIMTIME - 1;
    m_cycle = current_cycle;

    std::string m_data;

    if (current_cycle == 1) {
        m_data = '1' + std::to_string(STARTGREEN) + std::to_string(GREENTIME) + 
        std::to_string(YELLOWTIME) + std::to_string(REDTIME);
    } else {
        m_data = "0000000";
    }

    sc_din_link->send(new SST::Interfaces::StringEvent(
        std::to_string(keep_send) +
        std::to_string(keep_recv) +
        m_data +
        std::to_string(current_cycle)
    ));

    return false;

}
