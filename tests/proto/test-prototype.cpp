/*
 * Parent SST model
 *
 * */

#define RESET_TIME 3
#define SIMTIME 39

#include <sst/core/component.h>
#include <sst/core/elementinfo.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>
#include <sst/core/sst_config.h>

#include <bitset>

class prototype : public SST::Component {

public:

    prototype(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    void handle_galois_data_out(SST::Event *);

    void handle_fib_data_out(SST::Event *);

    bool tick(SST::Cycle_t);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        prototype, // class
        "proto", // element library
        "test", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "SST parent model",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "galois_lfsr_din", "Galois LFSR clock", {"sst.Interfaces.StringEvent"}},
        { "galois_lfsr_dout", "Galois LFSR data_out", {"sst.Interfaces.StringEvent"}},
        { "fib_lfsr_din", "Fibonacci LFSR reset", {"sst.Interfaces.StringEvent"}},
        { "fib_lfsr_dout", "Fibonacci LFSR data_out", {"sst.Interfaces.StringEvent"}},
    )

private:

    std::string m_clock;
    SST::Link *galois_din, *galois_dout, *fib_lfsr_din, *fib_lfsr_dout;

    // SST parameters
    SST::Output m_output;

    int m_cycle;

    unsigned char m_galois_lfsr, m_galois_lfsr_mask;
    unsigned char m_fib_lfsr;

};

prototype::prototype(SST::ComponentId_t id, SST::Params &params)
    : SST::Component(id),
      m_clock(params.find<std::string>("clock", "")),
      galois_din(configureLink("galois_lfsr_din")),
      galois_dout(configureLink(
          "galois_lfsr_dout", new SST::Event::Handler<prototype>(this, &prototype::handle_galois_data_out)
      )),
      fib_lfsr_din(configureLink("fib_lfsr_din")),
      fib_lfsr_dout(configureLink(
          "fib_lfsr_dout", new SST::Event::Handler<prototype>(this, &prototype::handle_fib_data_out)
      )) {

    m_output.init("\033[34mparent-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    if (!(galois_din && galois_dout && fib_lfsr_din && fib_lfsr_dout)) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure port\n");
    }

    registerClock(m_clock, new SST::Clock::Handler<prototype>(this, &prototype::tick));

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

void prototype::setup() {

    m_galois_lfsr = 1u;
    m_galois_lfsr_mask = 9u;
    m_fib_lfsr = 1u;
    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

}

void prototype::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());

}

void prototype::handle_galois_data_out(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {

        if ((m_cycle >= RESET_TIME) && (m_cycle % 2) && m_cycle < SIMTIME) {
            if (std::stoi(se->getString().c_str()) != m_galois_lfsr) {
                throw std::logic_error("Incorrect Galois LFSR value received");
            }
            unsigned lsb = m_galois_lfsr & 1u;
            m_galois_lfsr >>= 1;
            if (lsb) {
                m_galois_lfsr ^= m_galois_lfsr_mask;
            }
        }

    }
    delete ev;

}

void prototype::handle_fib_data_out(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {

        if ((m_cycle >= RESET_TIME) && (m_cycle % 2) && m_cycle < SIMTIME) {
            if (std::bitset<4>(se->getString()).to_ulong() != m_fib_lfsr) {
                throw std::logic_error("Incorrect Simple LFSR value received");
            }

            unsigned char bit = (((m_fib_lfsr >> 3) ^ ((m_fib_lfsr >> 0))) & 1u);
            m_fib_lfsr = (((m_fib_lfsr << 1) & 0x0Fu) | !bit);

        }

    }
    delete ev;

}

bool prototype::tick(SST::Cycle_t current_cycle) {

    std::string galois_reset_sig, fib_reset_sig;
    bool keep_send = current_cycle < SIMTIME;
    bool keep_recv = current_cycle < SIMTIME - 1;

    m_cycle = current_cycle;

    // turn reset off at 3 ns
    if (current_cycle >= RESET_TIME) {

        if (current_cycle == RESET_TIME) {
            m_output.verbose(CALL_INFO, 1, 0, "RESET OFF\n");
        }

        galois_reset_sig = "0";
        fib_reset_sig = "0";

    } else {

        galois_reset_sig = "1";
        fib_reset_sig = "1";

    }

    if (current_cycle == SIMTIME - 1) {
        m_output.verbose(CALL_INFO, 1, 0, "MODULE OFF\n");
    }

    galois_din->send(new SST::Interfaces::StringEvent(
        std::to_string(keep_send) +
        std::to_string(keep_recv) +
        galois_reset_sig +
        std::to_string(current_cycle)));
    fib_lfsr_din->send(new SST::Interfaces::StringEvent(
        std::to_string(keep_send) +
        std::to_string(keep_recv) +
        fib_reset_sig +
        std::to_string(current_cycle)));

    return false;

}
