#include "plague.hpp"
#include "setup.hpp"
#include "event_drivers.hpp"

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
