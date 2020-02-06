#ifndef PLAGUE_HPP
#define PLAGUE_HPP

#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#define SIMTIME 20
#define LOOPBEGIN 2
#define LOOPEND (SIMTIME - 3)
#define POPULATION_TOTAL 7760000000

class plague : public SST::Component {

public:

    plague(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    static void fix_signal_width(char, int, std::string &);

    void ceil_cure_thresh(SST::Event *);

    void ceil_pop_inf(SST::Event *);

    void ram(SST::Event *);

    void limit(SST::Event *);

    void exp_pop_inf(SST::Event *);

    void exp_br(SST::Event *);

    void pborn_today(SST::Event *);

    void randf_pop_inf(SST::Event *);

    void randf_sev(SST::Event *);

    void randf_inf(SST::Event *);

    void randf_let(SST::Event *);

    void ceil_pop_dead(SST::Event *);

    void min_let(SST::Event *);

    void min_inf(SST::Event *);

    void randf_br(SST::Event *);

    SST_ELI_REGISTER_COMPONENT(
            plague,
            "plague",
            "plague",
            SST_ELI_ELEMENT_VERSION(1, 0, 0),
            "Simulator for the plague",
            COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
            { "limit_din", "limit_din", { "sst.Interfaces.StringEvent" }},
            { "limit_dout", "limit_dout", { "sst.Interfaces.StringEvent" }},
            { "exp_pop_inf_din", "exp_pop_inf_din", { "sst.Interfaces.StringEvent" }},
            { "exp_pop_inf_dout", "exp_pop_inf_dout", { "sst.Interfaces.StringEvent" }},
            { "pinf_today_dout", "pinf_today_dout", { "sst.Interfaces.StringEvent" }},
            { "pinf_today_din", "pinf_today_din", { "sst.Interfaces.StringEvent" }},
            { "severity_din", "severity_din", { "sst.Interfaces.StringEvent" }},
            { "severity_dout", "severity_dout", { "sst.Interfaces.StringEvent" }},
            { "infectivity_din", "infectivity_din", { "sst.Interfaces.StringEvent" }},
            { "infectivity_dout", "infectivity_dout", { "sst.Interfaces.StringEvent" }},
            { "lethality_din", "lethality_din", { "sst.Interfaces.StringEvent" }},
            { "lethality_dout", "lethality_dout", { "sst.Interfaces.StringEvent" }},
            { "sc_ceil_pop_dead_din", "sc_ceil_pop_dead_din", { "sst.Interfaces.StringEvent" }},
            { "sc_ceil_pop_dead_dout", "sc_ceil_pop_dead_dout", { "sst.Interfaces.StringEvent" }},
            { "minf_let_din", "minf_let_din", { "sst.Interfaces.StringEvent" }},
            { "minf_let_dout", "minf_let_dout", { "sst.Interfaces.StringEvent" }},
            { "minf_inf_din", "minf_inf_din", { "sst.Interfaces.StringEvent" }},
            { "minf_inf_dout", "minf_inf_dout", { "sst.Interfaces.StringEvent" }},
            { "birth_rate_din", "birth_rate_din", { "sst.Interfaces.StringEvent" }},
            { "birth_rate_dout", "birth_rate_dout", { "sst.Interfaces.StringEvent" }},
            { "sc_ceil_cure_thresh_din", "sc_ceil_cure_thresh_din", { "sst.Interfaces.StringEvent" }},
            { "sc_ceil_cure_thresh_dout", "sc_ceil_cure_thresh_dout", { "sst.Interfaces.StringEvent" }},
            { "sc_ceil_pop_inf_din", "sc_ceil_pop_inf_din", { "sst.Interfaces.StringEvent" }},
            { "sc_ceil_pop_inf_dout", "sc_ceil_pop_inf_dout", { "sst.Interfaces.StringEvent" }},
            { "ram_din", "ram_din", { "sst.Interfaces.StringEvent" }},
            { "ram_dout", "ram_dout", { "sst.Interfaces.StringEvent" }},
    )

private:

    enum ADDRESSES {
        DAYS_ADDR = 2,
        TOTAL_INFECTED_ADDR,
        TOTAL_DEAD_ADDR,
        CURE_THRESHOLD_ADDR,
    };

    // SST parameters
    std::string m_clock;
    std::string seed_lim, seed_sev, seed_birth_rate, seed_let, seed_inf,
            seed_pop_born, seed_pop_inf, seed_evolve, seed_gene;

    // SST links and variables
    SST::Output m_output;
    SST::Link *ram_din_link, *ram_dout_link,
            *limit_din_link, *limit_dout_link,
            *exp_pop_inf_din_link, *exp_pop_inf_dout_link,
            *pinf_today_din_link, *pinf_today_dout_link,
            *infectivity_din_link, *infectivity_dout_link,
            *lethality_din_link, *lethality_dout_link,
            *severity_din_link, *severity_dout_link,
            *birth_rate_din_link, *birth_rate_dout_link,
            *sc_ceil_cure_thresh_din_link, *sc_ceil_cure_thresh_dout_link,
            *sc_ceil_pop_inf_din_link, *sc_ceil_pop_inf_dout_link,
            *sc_ceil_pop_dead_din_link, *sc_ceil_pop_dead_dout_link,
            *minf_let_din_link, *minf_let_dout_link,
            *minf_inf_din_link, *minf_inf_dout_link;

    bool keep_send, keep_recv;

    int BATCH_INFECTED, BATCH_BORN;

    float SEVERITY, INFECTIVITY, LETHALITY, BIRTH_RATE;
    float _POPULATION_INFECTED;
    std::string CURE_THRESHOLD,
            POPULATION_HEALTHY,
            POPULATION_INFECTED,
            TOTAL_INFECTED,
            POPULATION_DEAD,
            TOTAL_DEAD,
            POPULATION_AFFECTED;
    std::string LIMIT;

    unsigned int m_cycle{};

};

#endif
