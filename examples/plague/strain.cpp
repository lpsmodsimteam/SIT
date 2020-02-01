#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#define SIMTIME 20

class strain : public SST::Component {

public:

    strain(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    void handle_event_severity(SST::Event *);

    void handle_event_infectivity(SST::Event *);

    void handle_event_lethality(SST::Event *);

    void handle_event_minf(SST::Event *);

    SST_ELI_REGISTER_COMPONENT(
            strain,
            "plague",
            "strain",
            SST_ELI_ELEMENT_VERSION(1, 0, 0),
            "Traffic light simulator for the plague",
            COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
            { "severity_din", "severity_din", { "sst.Interfaces.StringEvent" }},
            { "severity_dout", "severity_dout", { "sst.Interfaces.StringEvent" }},
            { "infectivity_din", "infectivity_din", { "sst.Interfaces.StringEvent" }},
            { "infectivity_dout", "infectivity_dout", { "sst.Interfaces.StringEvent" }},
            { "lethality_din", "lethality_din", { "sst.Interfaces.StringEvent" }},
            { "lethality_dout", "lethality_dout", { "sst.Interfaces.StringEvent" }},
            { "minf_din", "minf_din", { "sst.Interfaces.StringEvent" }},
            { "minf_dout", "minf_dout", { "sst.Interfaces.StringEvent" }},
    )

private:

    // SST parameters
    std::string m_clock;
    // SST links and variables
    SST::Output m_output;
    SST::Link *infectivity_din_link, *infectivity_dout_link,
            *lethality_din_link, *lethality_dout_link,
            *severity_din_link, *severity_dout_link,
            *minf_din_link, *minf_dout_link;

    bool keep_send, keep_recv;
    unsigned int m_cycle{};

};

strain::strain(SST::ComponentId_t id, SST::Params &params) :
        SST::Component(id),
        // Collect all the parameters from the project driver
        m_clock(params.find<std::string>("CLOCK", "1Hz")),
        infectivity_din_link(configureLink("infectivity_din")),
        infectivity_dout_link(configureLink(
                "infectivity_dout",
                new SST::Event::Handler<strain>(this, &strain::handle_event_infectivity))),
        lethality_din_link(configureLink("lethality_din")),
        lethality_dout_link(configureLink(
                "lethality_dout",
                new SST::Event::Handler<strain>(this, &strain::handle_event_lethality))),
        severity_din_link(configureLink("severity_din")),
        severity_dout_link(configureLink(
                "severity_dout",
                new SST::Event::Handler<strain>(this, &strain::handle_event_severity))),
        minf_din_link(configureLink("minf_din")),
        minf_dout_link(configureLink(
                "minf_dout",
                new SST::Event::Handler<strain>(this, &strain::handle_event_minf)))
                 {

    m_output.init("\033[93mstrain-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    // Just register a plain clock for this simple example
    registerClock(m_clock, new SST::Clock::Handler<strain>(this, &strain::tick));

}

void strain::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

}

void strain::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());

}

void strain::handle_event_infectivity(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {

        std::cout << "INF " << se->getString() << '\n';
        minf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                se->getString() +
                "0.990000000"
        ));

    }

}

void strain::handle_event_severity(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {

        std::cout << "SEV " << se->getString() << '\n';
        // minf_din_link->send(new SST::Interfaces::StringEvent(
        //         "110.0000492760.000149276"
        // ));

    }

}

void strain::handle_event_minf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {

        std::cout << "MINF " << se->getString() << '\n';

    }

}

void strain::handle_event_lethality(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {

        std::cout << "LET " << se->getString() << '\n';
        minf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                se->getString() +
                "0.990000000"
        ));

    }

}

bool strain::tick(SST::Cycle_t current_cycle) {

    keep_send = current_cycle < SIMTIME;
    keep_recv = current_cycle < SIMTIME - 1;
    m_cycle = current_cycle;

    std::string m_data;

    if (current_cycle == 1) {

        severity_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "12340110999" +
                std::to_string(current_cycle)
        ));

        lethality_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "23450110999" +
                std::to_string(current_cycle)
        ));

        infectivity_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "34560110999" +
                std::to_string(current_cycle)
        ));

    } else {

        severity_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "12340110999" +
                std::to_string(current_cycle)
        ));

        lethality_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "23450110999" +
                std::to_string(current_cycle)
        ));

        infectivity_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "34560110999" +
                std::to_string(current_cycle)
        ));

    }

    return false;

}
