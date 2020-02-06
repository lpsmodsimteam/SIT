#ifndef PLAGUE_EVENT_DRIVERS_HPP
#define PLAGUE_EVENT_DRIVERS_HPP

#include "plague.hpp"
#include "setup.hpp"

#include <iomanip>

void plague::ram(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && keep_recv) {

        std::cout << "RAM " << m_cycle << ' ' << se->getString() << '\n';

    }

    delete se;

}

void plague::ceil_cure_thresh(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {

        CURE_THRESHOLD = se->getString();
        fix_signal_width('0', 8, CURE_THRESHOLD);

        std::string addr = std::to_string(ADDRESSES::CURE_THRESHOLD_ADDR);
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

void plague::limit(SST::Event *ev) {

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

void plague::randf_pop_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && keep_recv) {

        std::cout << "POPULATION INFECTED TODAY " << m_cycle << ' ' << se->getString() << '\n';
        BATCH_INFECTED = std::stoi(se->getString());

    }

    delete se;

}

void plague::randf_sev(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && keep_recv) {

        std::cout << "SEV " << se->getString() << '\n';
        SEVERITY = std::stof(se->getString());
    }

    delete se;

}

void plague::randf_br(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && keep_recv) {

        std::cout << "BIR " << se->getString() << '\n';
        BIRTH_RATE = std::stof(se->getString());
    }

    delete se;

}

void plague::randf_inf(SST::Event *ev) {

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

void plague::randf_let(SST::Event *ev) {

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

void plague::min_let(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_cycle < LOOPEND) {

        std::cout << "MIN LET " << se->getString() << '\n';
        LETHALITY = std::stof(se->getString());

    }

    delete se;

}

void plague::min_inf(SST::Event *ev) {

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

void plague::ceil_pop_inf(SST::Event *ev) {

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

void plague::ceil_pop_dead(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se && m_cycle < LOOPEND) {

        std::cout << "FINAL POPULATION DEAD " << se->getString() << '\n';

    }

    delete se;

}

void plague::exp_pop_inf(SST::Event *ev) {

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

#endif
