/*
 * Parent SST model
 *
 * */

#define WRITEMEM 10
#define SIMTIME 21

#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>
#include <sst/core/sst_config.h>

#include <bitset>

class unit : public SST::Component {

public:

    unit(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    void handle_ram_data_out(SST::Event *);

    bool tick(SST::Cycle_t);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        unit, // class
        "systemc", // element library
        "unit", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "SST parent model",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "ram_din", "Galois LFSR clock", { "sst.Interfaces.StringEvent" }},
        { "ram_dout", "Galois LFSR data_out", { "sst.Interfaces.StringEvent" }},
    )

private:

    std::string m_clock;
    SST::Link *ram_din, *ram_dout;

    // SST parameters
    SST::Output m_output;

    int m_cycle;


};

unit::unit(SST::ComponentId_t id, SST::Params &params)
    : SST::Component(id),
      m_clock(params.find<std::string>("clock", "")),
      ram_din(configureLink("ram_din")),
      ram_dout(configureLink(
          "ram_dout", new SST::Event::Handler<unit>(this, &unit::handle_ram_data_out)
      )) {

    m_output.init("\033[34mparent-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    if (!(ram_din && ram_dout)) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure port\n");
    }

    registerClock(m_clock, new SST::Clock::Handler<unit>(this, &unit::tick));

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

void unit::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

}

void unit::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());

}

void unit::handle_ram_data_out(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {

        std::cout << "data_out: " << std::bitset<8>(se->getString()).to_string() << std::endl;

    }
    delete ev;

}

bool unit::tick(SST::Cycle_t current_cycle) {

    bool keep_send = current_cycle < SIMTIME;
    bool keep_recv = current_cycle < SIMTIME - 1;

    bool cs = true, we, oe = true;
    std::string address = std::bitset<8>(current_cycle % 10).to_string();
    std::string data_in = std::bitset<8>(current_cycle % 10).to_string();

    we = current_cycle <= WRITEMEM;

    std::cout << "sending " << address << " " << data_in << std::endl;

    if (current_cycle == SIMTIME - 1) {
        m_output.verbose(CALL_INFO, 1, 0, "MODULE OFF\n");
    }

    ram_din->send(new SST::Interfaces::StringEvent(
        std::to_string(keep_send) +
        std::to_string(keep_recv) +
        address +
        std::to_string(cs) +
        std::to_string(we) +
        std::to_string(oe) +
        data_in +
        std::to_string(current_cycle)));

    return false;

}
