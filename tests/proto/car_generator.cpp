/**
Car Generator for carWash simulator

This component generates "cars" which are sent to the carwash
*/

#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/elementinfo.h>

#include <bitset>

#define RESET_TIME 3
#define SIMTIME 80

class car_generator : public SST::Component {

public:
    car_generator(SST::ComponentId_t id, SST::Params &params);

    void setup() override;

    void finish() override;

    void handle_galois_data_out(SST::Event *);

    void handle_fib_data_out(SST::Event *);

    bool tick(SST::Cycle_t);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        car_generator, // class
        "proto", // element library
        "car_generator", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Car Generator for the carwash",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Parameter name, description, default value
    SST_ELI_DOCUMENT_PARAMS(
        { "delay", "How long to wait between sending car events", "1MHz" },
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "car_type", "Port on which cars are sent", { "sst.Interfaces.StringEvent" }},
        { "galois_lfsr_din", "Galois LFSR m_clock", { "sst.Interfaces.StringEvent" }},
        { "galois_lfsr_dout", "Galois LFSR data_out", { "sst.Interfaces.StringEvent" }},
        { "fib_lfsr_din", "Fibonacci LFSR reset", { "sst.Interfaces.StringEvent" }},
        { "fib_lfsr_dout", "Fibonacci LFSR data_out", { "sst.Interfaces.StringEvent" }},
    )

private:

    int m_rand_int{};
    SST::Output m_output;
    std::string m_clock;
    SST::Link *car_type;
    SST::Link *galois_din, *galois_dout, *fib_lfsr_din, *fib_lfsr_dout;

};

car_generator::car_generator(SST::ComponentId_t id, SST::Params &params)
    : SST::Component(id),
      m_clock(params.find<std::string>("delay", "")),
      car_type(configureLink("car_type")),
      galois_din(configureLink("galois_lfsr_din")),
      galois_dout(configureLink(
          "galois_lfsr_dout", new SST::Event::Handler<car_generator>(this, &car_generator::handle_galois_data_out)
      )),
      fib_lfsr_din(configureLink("fib_lfsr_din")),
      fib_lfsr_dout(configureLink(
          "fib_lfsr_dout", new SST::Event::Handler<car_generator>(this, &car_generator::handle_fib_data_out)
      )) {

    m_output.init("\033[34mparent-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    // Register the clock
    registerClock(m_clock, new SST::Clock::Handler<car_generator>(this, &car_generator::tick));

    if (!(car_type && galois_din && galois_dout && fib_lfsr_din && fib_lfsr_dout)) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure car_type 'car_type'\n");
    }

}

void car_generator::setup() {
    m_rand_int = 0;
    m_output.verbose(CALL_INFO, 1, 0, "Component is being setup.\n");
}

void car_generator::finish() {
    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

void car_generator::handle_galois_data_out(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        m_rand_int += std::stoi(se->getString());
//        m_output.verbose(CALL_INFO, 1, 0, "galois_lfsr -> %s\n", );
    }
//    delete ev;

}

void car_generator::handle_fib_data_out(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        m_rand_int += std::bitset<4>(se->getString()).to_ulong();
//        m_output.verbose(CALL_INFO, 1, 0, "fib_lfsr -> %ld\n", std::bitset<4>(se->getString()).to_ulong());
    }
//    delete ev;

}

bool car_generator::tick(SST::Cycle_t current_cycle) {

    std::string galois_reset_sig, fib_reset_sig;
    bool keep_send = current_cycle < SIMTIME;
    bool keep_recv = current_cycle < SIMTIME - 1;

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

    // generating a random number between 0 and 2
    // 0 = No Car
    // 1 = Small Car
    // 2 = Large Car
    m_rand_int %= 3;
    car_type->send(new SST::Interfaces::StringEvent(std::to_string(m_rand_int)));
    m_rand_int = 0;

    return false;

}

