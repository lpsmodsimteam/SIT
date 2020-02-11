#ifndef PLAGUE_EVENT_DRIVERS_HPP
#define PLAGUE_EVENT_DRIVERS_HPP

#include "plague.hpp"
#include "setup.hpp"

#include <iomanip>

void plague::ram(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_ram_read_flag) {

        ram_data_out = se->getString();
        align_signal_width('0', 8, ram_data_out);

        std::string ram_addr = std::to_string(m_cycle % (SIMTIME / 2));
        align_signal_width('0', 8, ram_addr);
        fprintf(m_fp, "%s %s\n", ram_addr.c_str(), ram_data_out.c_str());

    }

    delete se;

}

void plague::mutation(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_keep_recv) {

        GENE = std::stoi(se->getString());
        switch (GENE) {
            case 0:
                // std::cout << "NEUTRAL\n";
                break;
            case 1:
                // std::cout << "GOOD\n";
                INFECTIVITY = abs(INFECTIVITY - RESEARCH);
                // std::cout << m_cycle << " INF (ABS) " << INFECTIVITY << '\n';
                m_mutate_lock = true;
                break;
            case 2:
                // std::cout << "BAD\n";
                CURE -= RESEARCH;
                break;
        }

    }

    delete se;

}

void plague::rng_limit(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_keep_recv) {

        LIMIT = se->getString();
        align_signal_width('0', 4, LIMIT);

        randf_sev_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "1" +
                seed_sev +
                "020" +
                LIMIT +
                std::to_string(m_cycle)
        ));

        randf_br_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "1" +
                seed_birth_rate +
                "020" +
                LIMIT +
                std::to_string(m_cycle)
        ));

    }

    delete se;

}

void plague::rng_mut(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_keep_recv) {

        MUTATION = se->getString();

    }

    delete se;

}

void plague::randf_rsrch(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_keep_recv) {

        RESEARCH = std::stof(se->getString());

    }

    delete se;

}

void plague::randf_sev(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_keep_recv) {

        SEVERITY = std::stof(se->getString());

    }

    delete se;

}

void plague::randf_br(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_keep_recv) {

        BIRTH_RATE = std::stof(se->getString());

    }

    delete se;

}

void plague::ceil_cure_thresh(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se && m_keep_recv) {

        std::string cure_threshold_str = se->getString();
        CURE_THRESHOLD = std::stoi(cure_threshold_str) / 100;

    }

    delete se;

}

void plague::randf_let(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 1;
    bool _keep_recv = m_cycle < SIMTIME - 2;

    if (se && m_keep_recv) {

        if (m_cycle == LOOPBEGIN) {

            LETHALITY = std::stof(se->getString());

        } else {

            min_let_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(_keep_send) +
                    std::to_string(_keep_recv) +
                    align_signal_width(10, LETHALITY + std::stof(se->getString())) +
                    "0.49"
            ));

        }

    }

    delete se;

}

void plague::min_let(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_cycle < LOOPEND) {

        LETHALITY = std::stof(se->getString());

    }

    delete se;

}

void plague::randf_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 1;
    bool _keep_recv = m_cycle < SIMTIME - 2;

    if (se && m_keep_recv) {

        if (m_cycle == LOOPBEGIN) {

            INFECTIVITY = std::stof(se->getString());

        } else {

            min_inf_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(_keep_send) +
                    std::to_string(_keep_recv) +
                    align_signal_width(10, INFECTIVITY + std::stof(se->getString())) +
                    "0.99"
            ));

        }

    }

    delete se;

}

void plague::min_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 2;
    bool _keep_recv = m_cycle < SIMTIME - 3;

    if (se && m_cycle < LOOPEND) {

        if (!m_mutate_lock) {
            INFECTIVITY = std::stof(se->getString());
        } else {
            m_mutate_lock = false;
        }

        exp_pop_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(_keep_send) +
                std::to_string(_keep_recv) +
                align_signal_width(10, INFECTIVITY)
        ));

    }

    delete se;

}

void plague::exp_pop_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 3;
    bool _keep_recv = m_cycle < SIMTIME - 4;

    if (se && m_cycle < LOOPEND - 1) {

        ceil_pop_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(_keep_send) +
                std::to_string(_keep_recv) +
                align_signal_width(2, std::stof(se->getString()) * BATCH_INFECTED)
        ));

    }

    delete se;

}

void plague::rng_pop_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_cycle < LOOPEND) {

        BATCH_INFECTED = std::stoi(se->getString());

    }

    delete se;

}

void plague::ceil_pop_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 4;
    bool _keep_recv = m_cycle < SIMTIME - 5;

    if (se && m_cycle < LOOPEND - 2) {

        TOTAL_INFECTED_TODAY = std::stoi(se->getString());

        ceil_pop_dead_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(_keep_send) +
                std::to_string(_keep_recv) +
                align_signal_width(2, TOTAL_INFECTED_TODAY * LETHALITY)
        ));

    }

    delete se;

}

void plague::ceil_pop_dead(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se && m_cycle < LOOPEND - 3) {
        TOTAL_DEAD_TODAY = std::stoi(se->getString());
    }

    delete se;

}

#endif
