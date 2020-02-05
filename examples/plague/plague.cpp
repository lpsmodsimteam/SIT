#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#include <iomanip>

#define SIMTIME 20
#define LOOPBEGIN 2
#define LOOPEND (SIMTIME - 3)
#define POPULATION_TOTAL 7760000000

enum ADDRESSES {
    DAYS = 2,
    TOTAL_INFECTED,
    TOTAL_DEAD,
    CURE_THRESHOLD,
};

class plague : public SST::Component {

public:

    plague(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    static void fix_signal_width(char, int, std::string &);

    void handle_event_sc_ceil_cure_thresh(SST::Event *);

    void handle_event_sc_ceil_pop_inf(SST::Event *);

    void handle_event_ram(SST::Event *);

    void handle_event_limit(SST::Event *);

    void handle_event_exp_pop_inf(SST::Event *);

    void handle_event_exp_br(SST::Event *);

    void handle_event_pborn_today(SST::Event *);

    void handle_event_pinf_today(SST::Event *);

    void handle_event_severity(SST::Event *);

    void handle_event_infectivity(SST::Event *);

    void handle_event_lethality(SST::Event *);

    void handle_event_sc_ceil_pop_dead(SST::Event *);

    void handle_event_minf_let(SST::Event *);

    void handle_event_minf_inf(SST::Event *);

    void handle_event_birth_rate(SST::Event *);

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
                new SST::Event::Handler<plague>(this, &plague::handle_event_ram))),
        limit_din_link(configureLink("limit_din")),
        limit_dout_link(configureLink(
                "limit_dout",
                new SST::Event::Handler<plague>(this, &plague::handle_event_limit))),
        exp_pop_inf_din_link(configureLink("exp_pop_inf_din")),
        exp_pop_inf_dout_link(configureLink(
                "exp_pop_inf_dout",
                new SST::Event::Handler<plague>(this, &plague::handle_event_exp_pop_inf))),
        pinf_today_din_link(configureLink("pinf_today_din")),
        pinf_today_dout_link(configureLink(
                "pinf_today_dout",
                new SST::Event::Handler<plague>(this, &plague::handle_event_pinf_today))),
        infectivity_din_link(configureLink("infectivity_din")),
        infectivity_dout_link(configureLink(
                "infectivity_dout",
                new SST::Event::Handler<plague>(this, &plague::handle_event_infectivity))),
        lethality_din_link(configureLink("lethality_din")),
        lethality_dout_link(configureLink(
                "lethality_dout",
                new SST::Event::Handler<plague>(this, &plague::handle_event_lethality))),
        severity_din_link(configureLink("severity_din")),
        severity_dout_link(configureLink(
                "severity_dout",
                new SST::Event::Handler<plague>(this, &plague::handle_event_severity))),
        birth_rate_din_link(configureLink("birth_rate_din")),
        birth_rate_dout_link(configureLink(
                "birth_rate_dout",
                new SST::Event::Handler<plague>(this, &plague::handle_event_birth_rate))),
        sc_ceil_cure_thresh_din_link(configureLink("sc_ceil_cure_thresh_din")),
        sc_ceil_cure_thresh_dout_link(configureLink(
                "sc_ceil_cure_thresh_dout",
                new SST::Event::Handler<plague>(this, &plague::handle_event_sc_ceil_cure_thresh))),
        sc_ceil_pop_inf_din_link(configureLink("sc_ceil_pop_inf_din")),
        sc_ceil_pop_inf_dout_link(configureLink(
                "sc_ceil_pop_inf_dout",
                new SST::Event::Handler<plague>(this, &plague::handle_event_sc_ceil_pop_inf))),
        sc_ceil_pop_dead_din_link(configureLink("sc_ceil_pop_dead_din")),
        sc_ceil_pop_dead_dout_link(configureLink(
                "sc_ceil_pop_dead_dout",
                new SST::Event::Handler<plague>(this, &plague::handle_event_sc_ceil_pop_dead))),
        minf_let_din_link(configureLink("minf_let_din")),
        minf_let_dout_link(configureLink(
                "minf_let_dout",
                new SST::Event::Handler<plague>(this, &plague::handle_event_minf_let))),
        minf_inf_din_link(configureLink("minf_inf_din")),
        minf_inf_dout_link(configureLink(
                "minf_inf_dout",
                new SST::Event::Handler<plague>(this, &plague::handle_event_minf_inf))) {

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

void plague::handle_event_ram(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && keep_recv) {

        std::cout << "RAM " << m_cycle << ' ' << se->getString() << '\n';

    }

    delete se;

}

void plague::handle_event_sc_ceil_cure_thresh(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {

        CURE_THRESHOLD = se->getString();
        fix_signal_width('0', 8, CURE_THRESHOLD);

        std::string addr = std::to_string(ADDRESSES::CURE_THRESHOLD);
        fix_signal_width('0', 8, addr);
        ram_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                addr +
                "111" +
                CURE_THRESHOLD
        ));

    }

    delete se;

}

void plague::handle_event_limit(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {

        if (m_cycle == 1) {
            std::cout << "LIM " << m_cycle << ' ' << se->getString() << '\n';
            LIMIT = se->getString();
            fix_signal_width('0', 4, LIMIT);
        }

    }

    delete se;

}

void plague::handle_event_pinf_today(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && keep_recv) {

        std::cout << "POPULATION INFECTED TODAY " << m_cycle << ' ' << se->getString() << '\n';
        BATCH_INFECTED = std::stoi(se->getString());

    }

    delete se;

}

void plague::handle_event_severity(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && keep_recv) {

        std::cout << "SEV " << se->getString() << '\n';
        SEVERITY = std::stof(se->getString());
    }

    delete se;

}

void plague::handle_event_birth_rate(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && keep_recv) {

        std::cout << "BIR " << se->getString() << '\n';
        BIRTH_RATE = std::stof(se->getString());
    }

    delete se;

}

void plague::handle_event_infectivity(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 1;
    bool _keep_recv = m_cycle < SIMTIME - 2;

    if (se && keep_recv) {

        if (m_cycle == LOOPBEGIN) {
            INFECTIVITY = std::stof(se->getString());
        } else {

            std::cout << "INF " << m_cycle << ' ' << se->getString() << '\n';
            std::ostringstream _data_out;
            _data_out << std::fixed << std::setprecision(10) << (INFECTIVITY + std::stof(se->getString()));
            minf_inf_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(_keep_send) +
                    std::to_string(_keep_recv) +
                    _data_out.str() +
                    "0.99"
            ));

        }

    }

    delete se;

}

void plague::handle_event_lethality(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 1;
    bool _keep_recv = m_cycle < SIMTIME - 2;

    if (se && keep_recv) {

        if (m_cycle == LOOPBEGIN) {
            LETHALITY = std::stof(se->getString());
        } else {

            std::cout << "LET " << m_cycle << ' ' << se->getString() << '\n';
            std::ostringstream _data_out;
            _data_out << std::fixed << std::setprecision(10) << (LETHALITY + std::stof(se->getString()));
            minf_let_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(_keep_send) +
                    std::to_string(_keep_recv) +
                    _data_out.str() +
                    "0.99"
            ));

        }

    }

    delete se;

}

void plague::handle_event_minf_let(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_cycle < LOOPEND) {

        std::cout << "MIN LET " << se->getString() << '\n';
        LETHALITY = std::stof(se->getString());

    }

    delete se;

}

void plague::handle_event_minf_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 2;
    bool _keep_recv = m_cycle < SIMTIME - 3;

    if (se && m_cycle < LOOPEND) {

        std::cout << "MIN INF " << se->getString() << '\n';
        INFECTIVITY = std::stof(se->getString());
        std::ostringstream _data_out;
        _data_out << std::fixed << std::setprecision(10) << INFECTIVITY;
        exp_pop_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(_keep_send) +
                std::to_string(_keep_recv) +
                _data_out.str()
        ));

    }

    delete se;

}

void plague::handle_event_sc_ceil_pop_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 4;
    bool _keep_recv = m_cycle < SIMTIME - 5;

    if (se && m_cycle < LOOPEND) {

        POPULATION_INFECTED = se->getString();
        std::cout << "FINAL POPULATION INFECTED " << POPULATION_INFECTED << '\n';
        std::ostringstream _data_out;
        _data_out << std::fixed << std::setprecision(2) << (std::stoi(POPULATION_INFECTED) * LETHALITY);
        sc_ceil_pop_dead_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(_keep_send) +
                std::to_string(_keep_recv) +
                _data_out.str()
        ));

    }

    delete se;

}

void plague::handle_event_sc_ceil_pop_dead(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se && m_cycle < LOOPEND) {

        std::cout << "FINAL POPULATION DEAD " << se->getString() << '\n';

    }

    delete se;

}

void plague::handle_event_exp_pop_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 3;
    bool _keep_recv = m_cycle < SIMTIME - 4;

    if (se && m_cycle < LOOPEND) {

        std::cout << "EXP INF " << se->getString() << '\n';
        _POPULATION_INFECTED = std::stof(se->getString()) * BATCH_INFECTED;
        std::cout << "POPULATION_INFECTED " << _POPULATION_INFECTED << '\n';
        std::ostringstream _data_out;
        _data_out << std::fixed << std::setprecision(2) << _POPULATION_INFECTED;
        sc_ceil_pop_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(_keep_send) +
                std::to_string(_keep_recv) +
                _data_out.str()
        ));

    }

    delete se;

}

void plague::fix_signal_width(const char chr, int width, std::string &signal) {
    int _len = signal.length();
    if (_len < width) {
        signal = std::string(width - _len, chr).append(signal);
    }
}

bool plague::tick(SST::Cycle_t current_cycle) {

    keep_send = current_cycle < SIMTIME;
    keep_recv = current_cycle < SIMTIME - 1;
    m_cycle = current_cycle;

    std::string m_data;
    std::string cycle = std::to_string(current_cycle);
    fix_signal_width('0', 8, cycle);

    ram_din_link->send(new SST::Interfaces::StringEvent(
            std::to_string(keep_send) +
            std::to_string(keep_recv) +
            cycle +
            "101" +
            "00000000"
    ));

    if (current_cycle == 1) {

        limit_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "1" +
                seed_lim +
                "0201000" +
                std::to_string(current_cycle)
        ));

    }

    if (current_cycle > 1) {

        if (current_cycle == 2) {

            severity_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "1" +
                    seed_sev +
                    "002" +
                    LIMIT +
                    std::to_string(current_cycle)
            ));

            birth_rate_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "1" +
                    seed_birth_rate +
                    "002" +
                    LIMIT +
                    std::to_string(current_cycle)
            ));

        } else if (current_cycle == 3) {

            std::ostringstream _data_out;
            _data_out << std::fixed << std::setprecision(2) << (SEVERITY * LETHALITY * POPULATION_TOTAL);
            sc_ceil_cure_thresh_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    _data_out.str()
            ));

        } else if (!keep_recv) {

            severity_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "0" +
                    seed_sev +
                    "002" +
                    std::to_string(current_cycle)
            ));

            birth_rate_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "0" +
                    seed_birth_rate +
                    "002" +
                    std::to_string(current_cycle)
            ));

            limit_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "0" +
                    seed_lim +
                    "0201000" +
                    std::to_string(current_cycle)
            ));

            pinf_today_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "0" +
                    seed_pop_inf +
                    "0020100" +
                    std::to_string(current_cycle)
            ));

            lethality_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "0" +
                    seed_let +
                    "002" +
                    LIMIT +
                    std::to_string(current_cycle)
            ));

            infectivity_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "0" +
                    seed_inf +
                    "002" +
                    LIMIT +
                    std::to_string(current_cycle)
            ));

            sc_ceil_cure_thresh_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "0000000"
            ));

        }

        pinf_today_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "1" +
                seed_pop_inf +
                "0020100" +
                std::to_string(current_cycle)
        ));

        lethality_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "1" +
                seed_let +
                "002" +
                LIMIT +
                std::to_string(current_cycle)
        ));

        infectivity_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "1" +
                seed_inf +
                "002" +
                LIMIT +
                std::to_string(current_cycle)
        ));

    }

    return false;

}
