#ifndef MONTE_CARLO_HPP
#define MONTE_CARLO_HPP

#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

class monte_carlo : public SST::Component {

public:
    monte_carlo(SST::ComponentId_t, SST::Params&);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    void x_rand32(SST::Event*);

    void y_rand32(SST::Event*);

    void div_x(SST::Event*);

    void div_y(SST::Event*);

    void sum_sq(SST::Event*);

    SST_ELI_REGISTER_COMPONENT(monte_carlo, "monte_carlo", "monte_carlo",
                               SST_ELI_ELEMENT_VERSION(1, 0, 0),
                               "Simulator for the monte_carlo",
                               COMPONENT_CATEGORY_UNCATEGORIZED)

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        {"x_rand32_din", "x_rand32_din", {"sst.Interfaces.StringEvent"}},
        {"x_rand32_dout", "x_rand32_dout", {"sst.Interfaces.StringEvent"}},
        {"y_rand32_din", "y_rand32_din", {"sst.Interfaces.StringEvent"}},
        {"y_rand32_dout", "y_rand32_dout", {"sst.Interfaces.StringEvent"}},
        {"div_x_din", "div_x_din", {"sst.Interfaces.StringEvent"}},
        {"div_x_dout", "div_x_dout", {"sst.Interfaces.StringEvent"}},
        {"div_y_din", "div_y_din", {"sst.Interfaces.StringEvent"}},
        {"div_y_dout", "div_y_dout", {"sst.Interfaces.StringEvent"}},
        {"sum_sq_din", "sum_sq_din", {"sst.Interfaces.StringEvent"}},
        {"sum_sq_dout", "sum_sq_dout", {"sst.Interfaces.StringEvent"}}, )

private:
    // SST parameters
    uint16_t seed;
    bool output;

    // SST attributes
    SST::Output m_output;
    SST::Link *x_rand32_din_link, *x_rand32_dout_link, *y_rand32_din_link,
        *y_rand32_dout_link, *div_x_din_link, *div_x_dout_link, *div_y_din_link,
        *div_y_dout_link, *sum_sq_din_link, *sum_sq_dout_link;

    bool m_keep_send{}, m_keep_recv{};
    float x{}, y{}, m_fatality{}, m_birth_rate{}, m_cure{}, m_research{};
};

#endif // MONTE_CARLO_HPP
