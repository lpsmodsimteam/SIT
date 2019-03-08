#ifndef SST_DEV_HPP
#define SST_DEV_HPP

#include <unistd.h>

#include <sst/core/component.h>
#include <sst/core/elementinfo.h>
#include <sst/core/link.h>
#include <sst/core/interfaces/stringEvent.h>


class sst_dev : public SST::Component {

public:

    sst_dev(SST::ComponentId_t, SST::Params &);

    ~sst_dev() override;

    void setup() override;

    void finish() override;

    void handle_galois_data_out(SST::Event *);

    void handle_fib_lfsr(SST::Event *);

    bool tick(SST::Cycle_t);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        sst_dev, // class
        "proto", // element library
        "sst_dev", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "gdfjngkdfnglkdfnkj",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "galois_clock", "Galois LFSR clock", { "sst.Interfaces.StringEvent" }},
        { "galois_reset", "Galois LFSR reset", { "sst.Interfaces.StringEvent" }},
        { "galois_data_out", "Galois LFSR data_out", { "sst.Interfaces.StringEvent" }},
        { "link_fib", "Port blalalal", { "sst.Interfaces.StringEvent" }}
    )

private:

    std::string m_clock;
    SST::Link *galois_reset, *galois_clock, *galois_data_out, *link_fib;

    // SST parameters
    SST::Output m_output;

};

#endif
