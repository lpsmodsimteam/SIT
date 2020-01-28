#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#define SIMTIME 86400

class strain_stats : public SST::Component {

public:

    strain_stats(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    void handle_event(SST::Event *);

    SST_ELI_REGISTER_COMPONENT(
        strain_stats,
        "intersection",
        "strain_stats",
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Traffic light simulator for the intersection",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
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
    SST::Link *infectivity, *lethality, *severity;

    unsigned int m_cycle{};

};

strain_stats::strain_stats(SST::ComponentId_t id, SST::Params &params) :
    SST::Component(id),
    // Collect all the parameters from the project driver
    m_clock(params.find<std::string>("CLOCK", "1Hz")),
    sc_dout_link(configureLink("infectivity_din"),
    sc_dout_link(configureLink(
        "infectivity_dout",
        new SST::Event::Handler<strain_stats>(this, &strain_stats::handle_event))),
    sc_dout_link(configureLink("severity_din")),
    sc_dout_link(configureLink(
        "severity_dout",
        new SST::Event::Handler<strain_stats>(this, &strain_stats::handle_event))),
    sc_dout_link(configureLink("lethality_din")),
    sc_dout_link(configureLink(
        "lethality_dout",
        new SST::Event::Handler<strain_stats>(this, &strain_stats::handle_event))) {

    m_output.init("\033[93mstrain_stats-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    // Check parameters
    if (!(GREENTIME && YELLOWTIME && REDTIME)) {
        m_output.fatal(CALL_INFO, -1, "Error: times must be greater than zero.\n");
    }

    // Print parameters
    m_output.verbose(CALL_INFO, 1, 0, "GREENTIME=%d, YELLOWTIME=%d, REDTIME=%d, STARTGREEN=%d\n",
                     GREENTIME, YELLOWTIME, REDTIME, STARTGREEN);

    // Just register a plain clock for this simple example
    registerClock(m_clock, new SST::Clock::Handler<strain_stats>(this, &strain_stats::tick));

    // Configure our ports
    if (!light_state) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure light_state 'light_state'\n");
    }

}

void strain_stats::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

}

void strain_stats::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());

}

void strain_stats::handle_event(SST::Event *ev) {

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
bool strain_stats::tick(SST::Cycle_t current_cycle) {

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
