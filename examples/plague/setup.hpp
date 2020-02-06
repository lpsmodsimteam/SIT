#ifndef PLAGUE_SETUP_HPP
#define PLAGUE_SETUP_HPP

#include "plague.hpp"

plague::plague(SST::ComponentId_t id, SST::Params &params) :
        SST::Component(id),
        // Collect all the parameters from the project driver
        m_clock(params.find<std::string>("CLOCK", "1Hz")),
        seed_lim(params.find<std::string>("SEED0", "12345")),
        seed_sev(params.find<std::string>("SEED1", "12346")),
        seed_birth_rate(params.find<std::string>("SEED2", "12347")),
        seed_let(params.find<std::string>("SEED3", "12348")),
        seed_inf(params.find<std::string>("SEED4", "12349")),
        seed_pop_inf(params.find<std::string>("SEED5", "12351")),
        seed_evolve(params.find<std::string>("SEED6", "12352")),
        seed_gene(params.find<std::string>("SEED7", "12353")),
        ram_din_link(configureLink("ram_din")),
        ram_dout_link(configureLink(
                "ram_dout",
                new SST::Event::Handler<plague>(this, &plague::ram))
        ),
        limit_din_link(configureLink("limit_din")),
        limit_dout_link(configureLink(
                "limit_dout",
                new SST::Event::Handler<plague>(this, &plague::limit))
        ),
        exp_pop_inf_din_link(configureLink("exp_pop_inf_din")),
        exp_pop_inf_dout_link(configureLink(
                "exp_pop_inf_dout",
                new SST::Event::Handler<plague>(this, &plague::exp_pop_inf))
        ),
        pinf_today_din_link(configureLink("pinf_today_din")),
        pinf_today_dout_link(configureLink(
                "pinf_today_dout",
                new SST::Event::Handler<plague>(this, &plague::randf_pop_inf))
        ),
        infectivity_din_link(configureLink("infectivity_din")),
        infectivity_dout_link(configureLink(
                "infectivity_dout",
                new SST::Event::Handler<plague>(this, &plague::randf_inf))
        ),
        lethality_din_link(configureLink("lethality_din")),
        lethality_dout_link(configureLink(
                "lethality_dout",
                new SST::Event::Handler<plague>(this, &plague::randf_let))
        ),
        severity_din_link(configureLink("severity_din")),
        severity_dout_link(configureLink(
                "severity_dout",
                new SST::Event::Handler<plague>(this, &plague::randf_sev))
        ),
        birth_rate_din_link(configureLink("birth_rate_din")),
        birth_rate_dout_link(configureLink(
                "birth_rate_dout",
                new SST::Event::Handler<plague>(this, &plague::randf_br))
        ),
        sc_ceil_cure_thresh_din_link(configureLink("sc_ceil_cure_thresh_din")),
        sc_ceil_cure_thresh_dout_link(configureLink(
                "sc_ceil_cure_thresh_dout",
                new SST::Event::Handler<plague>(this, &plague::ceil_cure_thresh))
        ),
        sc_ceil_pop_inf_din_link(configureLink("sc_ceil_pop_inf_din")),
        sc_ceil_pop_inf_dout_link(configureLink(
                "sc_ceil_pop_inf_dout",
                new SST::Event::Handler<plague>(this, &plague::ceil_pop_inf))
        ),
        sc_ceil_pop_dead_din_link(configureLink("sc_ceil_pop_dead_din")),
        sc_ceil_pop_dead_dout_link(configureLink(
                "sc_ceil_pop_dead_dout",
                new SST::Event::Handler<plague>(this, &plague::ceil_pop_dead))
        ),
        minf_let_din_link(configureLink("minf_let_din")),
        minf_let_dout_link(configureLink(
                "minf_let_dout",
                new SST::Event::Handler<plague>(this, &plague::min_let))
        ),
        minf_inf_din_link(configureLink("minf_inf_din")),
        minf_inf_dout_link(configureLink(
                "minf_inf_dout",
                new SST::Event::Handler<plague>(this, &plague::min_inf))
        ) {

    m_output.init("\033[93mplague-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    // Just register a plain clock for this simple example
    registerClock(m_clock, new SST::Clock::Handler<plague>(this, &plague::tick));

}

void plague::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

}

void plague::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());

}

void plague::fix_signal_width(const char chr, int width, std::string &signal) {
    int _len = signal.length();
    if (_len < width) {
        signal = std::string(width - _len, chr).append(signal);
    }
}

#endif
