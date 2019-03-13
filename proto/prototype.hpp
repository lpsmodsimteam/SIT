#ifndef SST_DEV_HPP
#define SST_DEV_HPP

#include <unistd.h>

#include <sst/core/component.h>
#include <sst/core/elementinfo.h>
#include <sst/core/link.h>
#include <sst/core/interfaces/stringEvent.h>


class prototype : public SST::Component {

public:

    prototype(SST::ComponentId_t, SST::Params &);

    ~prototype() override;

    void setup() override;

    void finish() override;

    void handle_galois_data_out(SST::Event *);

    void handle_fib_data_out(SST::Event *);

    bool tick(SST::Cycle_t);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        prototype, // class
        "proto", // element library
        "prototype", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "SST parent model",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "galois_din", "Galois LFSR clock", { "sst.Interfaces.StringEvent" }},
        { "galois_dout", "Galois LFSR data_out", { "sst.Interfaces.StringEvent" }},
        { "fib_din", "Fibonacci LFSR reset", { "sst.Interfaces.StringEvent" }},
        { "fib_dout", "Fibonacci LFSR data_out", { "sst.Interfaces.StringEvent" }},
    )

private:

    std::string m_clock;
    SST::Link *galois_din, *galois_dout,
        *fib_din, *fib_dout;

    // SST parameters
    SST::Output m_output;

};

#endif
