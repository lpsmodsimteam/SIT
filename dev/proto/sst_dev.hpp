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

    void handleEvent(SST::Event *);

    bool tick(SST::Cycle_t);

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
            sst_dev, // class
            "proto1", // element library
            "sst_dev", // component
            SST_ELI_ELEMENT_VERSION(1, 0, 0),
            "Simple 4-bit Galois Linear Feedback Shift Register",
            COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
            { "port", "Port on which to receive cars", {"sst.Interfaces.StringEvent"}}
    )

private:

    SST::Link *port;

    // SST parameters
    SST::Output m_output;

};

#endif
