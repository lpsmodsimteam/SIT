#ifndef PLAGUE_HPP
#define PLAGUE_HPP

#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#define POPULATION_TOTAL 7760000000

class plague : public SST::Component {

public:

    plague(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    static void align_signal_width(char, int, std::string &);

    static std::string align_signal_width(int, float);

    void flash_mem(SST::Event *);

    void mutation(SST::Event *);

    void rng_limit(SST::Event *);

    void rng_pop_inf(SST::Event *);

    void rng_mut(SST::Event *);

    void randf_br(SST::Event *);

    void randf_rsrch(SST::Event *);

    void randf_sev(SST::Event *);

    void randf_inf(SST::Event *);

    void randf_fat(SST::Event *);

    void min_fat(SST::Event *);

    void min_inf(SST::Event *);

    void ceil_cure_thresh(SST::Event *);

    void ceil_pop_inf(SST::Event *);

    void ceil_pop_dead(SST::Event *);

    void exp_pop_inf(SST::Event *);

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
            { "rng_limit_din", "rng_limit_din", { "sst.Interfaces.StringEvent" }},
            { "rng_limit_dout", "rng_limit_dout", { "sst.Interfaces.StringEvent" }},
            { "rng_pop_inf_dout", "rng_pop_inf_dout", { "sst.Interfaces.StringEvent" }},
            { "rng_pop_inf_din", "rng_pop_inf_din", { "sst.Interfaces.StringEvent" }},
            { "rng_mut_dout", "rng_mut_dout", { "sst.Interfaces.StringEvent" }},
            { "rng_mut_din", "rng_mut_din", { "sst.Interfaces.StringEvent" }},
            { "exp_pop_inf_din", "exp_pop_inf_din", { "sst.Interfaces.StringEvent" }},
            { "exp_pop_inf_dout", "exp_pop_inf_dout", { "sst.Interfaces.StringEvent" }},
            { "randf_sev_din", "randf_sev_din", { "sst.Interfaces.StringEvent" }},
            { "randf_sev_dout", "randf_sev_dout", { "sst.Interfaces.StringEvent" }},
            { "randf_inf_din", "randf_inf_din", { "sst.Interfaces.StringEvent" }},
            { "randf_inf_dout", "randf_inf_dout", { "sst.Interfaces.StringEvent" }},
            { "randf_fat_din", "randf_fat_din", { "sst.Interfaces.StringEvent" }},
            { "randf_fat_dout", "randf_fat_dout", { "sst.Interfaces.StringEvent" }},
            { "randf_br_din", "randf_br_din", { "sst.Interfaces.StringEvent" }},
            { "randf_br_dout", "randf_br_dout", { "sst.Interfaces.StringEvent" }},
            { "randf_rsrch_din", "randf_rsrch_din", { "sst.Interfaces.StringEvent" }},
            { "randf_rsrch_dout", "randf_rsrch_dout", { "sst.Interfaces.StringEvent" }},
            { "ceil_pop_dead_din", "ceil_pop_dead_din", { "sst.Interfaces.StringEvent" }},
            { "ceil_pop_dead_dout", "ceil_pop_dead_dout", { "sst.Interfaces.StringEvent" }},
            { "min_fat_din", "min_fat_din", { "sst.Interfaces.StringEvent" }},
            { "min_fat_dout", "min_fat_dout", { "sst.Interfaces.StringEvent" }},
            { "min_inf_din", "min_inf_din", { "sst.Interfaces.StringEvent" }},
            { "min_inf_dout", "min_inf_dout", { "sst.Interfaces.StringEvent" }},
            { "ceil_cure_thresh_din", "ceil_cure_thresh_din", { "sst.Interfaces.StringEvent" }},
            { "ceil_cure_thresh_dout", "ceil_cure_thresh_dout", { "sst.Interfaces.StringEvent" }},
            { "ceil_pop_inf_din", "ceil_pop_inf_din", { "sst.Interfaces.StringEvent" }},
            { "ceil_pop_inf_dout", "ceil_pop_inf_dout", { "sst.Interfaces.StringEvent" }},
            { "flash_mem_din", "flash_mem_din", { "sst.Interfaces.StringEvent" }},
            { "flash_mem_dout", "flash_mem_dout", { "sst.Interfaces.StringEvent" }},
            { "mutation_din", "mutation_din", { "sst.Interfaces.StringEvent" }},
            { "mutation_dout", "mutation_dout", { "sst.Interfaces.StringEvent" }},
    )

private:

    // SST parameters
    std::string m_clock;
    std::string seed_lim, seed_sev, seed_birth_rate, seed_let, seed_inf,
            seed_pop_inf, seed_research, seed_mutation;

    // SST links and variables
    SST::Output m_output;
    SST::Link *flash_mem_din_link, *flash_mem_dout_link,
            *mutation_din_link, *mutation_dout_link,
            *rng_limit_din_link, *rng_limit_dout_link,
            *rng_pop_inf_din_link, *rng_pop_inf_dout_link,
            *rng_mut_din_link, *rng_mut_dout_link,
            *randf_inf_din_link, *randf_inf_dout_link,
            *randf_fat_din_link, *randf_fat_dout_link,
            *randf_sev_din_link, *randf_sev_dout_link,
            *randf_br_din_link, *randf_br_dout_link,
            *randf_rsrch_din_link, *randf_rsrch_dout_link,
            *exp_pop_inf_din_link, *exp_pop_inf_dout_link,
            *ceil_cure_thresh_din_link, *ceil_cure_thresh_dout_link,
            *ceil_pop_inf_din_link, *ceil_pop_inf_dout_link,
            *ceil_pop_dead_din_link, *ceil_pop_dead_dout_link,
            *min_fat_din_link, *min_fat_dout_link,
            *min_inf_din_link, *min_inf_dout_link;

    unsigned int SIMTIME = 50000;
    unsigned int LOOPBEGIN = 2;
    unsigned int LOOPEND = (SIMTIME - 2);
    bool m_keep_send{}, m_keep_recv{};

    unsigned int m_cure_threshold{}, m_batch_infected{}, m_total_infected{}, m_total_dead{}, m_total_infected_today{}, m_total_dead_today{}, m_gene{};
    float m_severity{}, m_infectivity{}, m_fatality{}, m_birth_rate{}, m_cure{}, m_research{};
    std::string m_limit, m_mutation = "0";
    bool m_mutate_lock = false, m_loop_lock = true, m_mem_read_flag = false;

    // flash memory variables
    std::string m_mem_read = "101", m_mem_write = "111", m_mem_data_out;

    unsigned int m_cycle{};
    FILE *m_fp{};

};

#endif
