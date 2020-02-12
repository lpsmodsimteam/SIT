#ifndef PLAGUE_EVENT_DRIVERS_HPP
#define PLAGUE_EVENT_DRIVERS_HPP

#include "plague.hpp"
#include "setup.hpp"

#include <iomanip>

void plague::flash_mem(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_mem_read_flag) {

        m_mem_data_out = se->getString();
        align_signal_width('0', 8, m_mem_data_out);

        std::string ram_addr = std::to_string(m_cycle % (SIMTIME / 2));
        align_signal_width('0', 8, ram_addr);
        fprintf(m_fp, "%s %s\n", ram_addr.c_str(), m_mem_data_out.c_str());

    }

    delete se;

}

void plague::mutation(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_keep_recv) {

        m_gene = std::stoi(se->getString());
        switch (m_gene) {
            case 0:
                // std::cout << "NEUTRAL\n";
                break;
            case 1:
                // std::cout << "GOOD\n";
                m_infectivity = abs(m_infectivity - m_research);
                // std::cout << m_cycle << " INF (ABS) " << INFECTIVITY << '\n';
                m_mutate_lock = true;
                break;
            case 2:
                // std::cout << "BAD\n";
                m_cure -= m_research;
                break;
        }

    }

    delete se;

}

void plague::rng_limit(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_keep_recv) {

        m_limit = se->getString();
        align_signal_width('0', 4, m_limit);

        randf_sev_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "1" +
                seed_sev +
                "020" +
                m_limit +
                std::to_string(m_cycle)
        ));

        randf_br_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "1" +
                seed_birth_rate +
                "020" +
                m_limit +
                std::to_string(m_cycle)
        ));

    }

    delete se;

}

void plague::rng_mut(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_keep_recv) {

        m_mutation = se->getString();

    }

    delete se;

}

void plague::randf_rsrch(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_keep_recv) {

        m_research = std::stof(se->getString());

    }

    delete se;

}

void plague::randf_sev(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_keep_recv) {

        m_severity = std::stof(se->getString());

    }

    delete se;

}

void plague::randf_br(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_keep_recv) {

        m_birth_rate = std::stof(se->getString());

    }

    delete se;

}

void plague::ceil_cure_thresh(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se && m_keep_recv) {

        std::string cure_threshold_str = se->getString();
        m_cure_threshold = std::stoi(cure_threshold_str) / 100;

    }

    delete se;

}

void plague::randf_fat(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 1;
    bool _keep_recv = m_cycle < SIMTIME - 2;

    if (se && m_keep_recv) {

        if (m_cycle == LOOPBEGIN) {

            m_fatality = std::stof(se->getString());

        } else {

            min_fat_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(_keep_send) +
                    std::to_string(_keep_recv) +
                    align_signal_width(10, m_fatality + std::stof(se->getString())) +
                    "0.49"
            ));

        }

    }

    delete se;

}

void plague::min_fat(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_cycle < LOOPEND) {

        m_fatality = std::stof(se->getString());

    }

    delete se;

}

void plague::randf_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 1;
    bool _keep_recv = m_cycle < SIMTIME - 2;

    if (se && m_keep_recv) {

        if (m_cycle == LOOPBEGIN) {

            m_infectivity = std::stof(se->getString());

        } else {

            min_inf_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(_keep_send) +
                    std::to_string(_keep_recv) +
                    align_signal_width(10, m_infectivity + std::stof(se->getString())) +
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
            m_infectivity = std::stof(se->getString());
        } else {
            m_mutate_lock = false;
        }

        exp_pop_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(_keep_send) +
                std::to_string(_keep_recv) +
                align_signal_width(10, m_infectivity)
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
                align_signal_width(2, std::stof(se->getString()) * m_batch_infected)
        ));

    }

    delete se;

}

void plague::rng_pop_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_cycle < LOOPEND) {

        m_batch_infected = std::stoi(se->getString());

    }

    delete se;

}

void plague::ceil_pop_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 4;
    bool _keep_recv = m_cycle < SIMTIME - 5;

    if (se && m_cycle < LOOPEND - 2) {

        m_total_infected_today = std::stoi(se->getString());

        ceil_pop_dead_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(_keep_send) +
                std::to_string(_keep_recv) +
                align_signal_width(2, m_total_infected_today * m_fatality)
        ));

    }

    delete se;

}

void plague::ceil_pop_dead(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se && m_cycle < LOOPEND - 3) {
        m_total_dead_today = std::stoi(se->getString());
    }

    delete se;

}

#endif
