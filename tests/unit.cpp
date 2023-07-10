/*
 * Parent SST model
 *
 * */

#define WRITEMEM 11
#define SIMTIME 25

#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#include <sst/sit/bufwidth.hpp>
#include <sst/sit/linkwrap.hpp>

#ifndef ELEM_LIB
#define ELEM_LIB ""
#endif

class unit : public SST::Component {
   public:
    unit(SST::ComponentId_t, SST::Params&);

    void setup() override;

    void finish() override;

    void handle_ram_data_out(SST::Event*);

    bool tick(SST::Cycle_t);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        unit,      // class
        ELEM_LIB,  // element library
        "unit",    // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "SST parent model",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        {"ram_din", "RAM data_in", {"sst.Interfaces.StringEvent"}},
        {"ram_dout", "RAM data_out", {"sst.Interfaces.StringEvent"}},
    )

   private:
    std::string m_clock;

    // SST parameters
    SST::Output m_output;

    // SST attributes
    LinkWrapper* ram_link;

    unsigned int m_cycle{};
    bool m_keep_send{}, m_keep_recv{};
    FILE* m_fp{};
    char m_message[WRITEMEM + 1] = "HELLO WORLD";
};

unit::unit(SST::ComponentId_t id, SST::Params& params)
    : SST::Component(id), m_clock(params.find<std::string>("clock", "")) {
    m_output.init(
        "\033[34mparent-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT
    );

    ram_link = new LinkWrapper(&m_keep_send, &m_keep_recv);
    ram_link->set_din_link(configureLink("ram_din"));
    ram_link->set_dout_link(configureLink(
        "ram_dout",
        new SST::Event::Handler<unit>(this, &unit::handle_ram_data_out)
    ));
    // ram_link->set_buffer_lengths(11, 4);

    registerClock(m_clock, new SST::Clock::Handler<unit>(this, &unit::tick));

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();
}

void unit::setup() {
    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");
    m_fp = std::fopen("memory_dump.txt", "w");
}

void unit::finish() {
    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());
    std::fclose(m_fp);
}
void unit::handle_ram_data_out(SST::Event* ev) {
    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (se) {
        if ((m_cycle > WRITEMEM) && (m_cycle < SIMTIME - 2)) {
            std::string data_out = align_buffer_width(se->getString(), 8);
            fprintf(m_fp, "%s\n", data_out.c_str());
        }
    }
    delete ev;
}

bool unit::tick(SST::Cycle_t current_cycle) {
    m_keep_send = current_cycle < SIMTIME;
    m_keep_recv = current_cycle < SIMTIME - 1;

    bool cs = true, we, oe = true;
    std::string address =
        align_buffer_width(std::to_string((current_cycle - 1) % WRITEMEM), 3);

    std::string data_in;
    if ((current_cycle - 1) < WRITEMEM) {
        data_in = align_buffer_width(
            std::to_string(int(m_message[current_cycle - 1])), 3
        );
    } else {
        data_in = "000";
    }

    we = current_cycle <= WRITEMEM;
    m_cycle = current_cycle;

    ram_link->send(
        address + std::to_string(cs) + std::to_string(we) + std::to_string(oe) +
        data_in
    );

    return false;
}
