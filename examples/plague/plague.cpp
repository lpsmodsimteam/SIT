#include "plague.hpp"
#include "setup.hpp"
#include "event_drivers.hpp"

bool plague::tick(SST::Cycle_t current_cycle) {

    keep_send = current_cycle < SIMTIME;
    keep_recv = current_cycle < SIMTIME - 1;
    m_cycle = current_cycle;

    // RAM COMPONENT
    std::string dont_care_addr = "00000000", ram_cycle = std::to_string(current_cycle);
    align_signal_width('0', 8, ram_cycle);

    ram_din_link->send(new SST::Interfaces::StringEvent(
            std::to_string(keep_send) +
            std::to_string(keep_recv) +
            ram_cycle +
            m_ram_noop +
            dont_care_addr
    ));

    if (current_cycle == 1) {

        rng_limit_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "1" +
                seed_lim +
                "0201000" +
                std::to_string(current_cycle)
        ));

    }

    if (current_cycle == LOOPBEGIN + 1) {

        ceil_cure_thresh_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                align_signal_width(2, SEVERITY * LETHALITY * POPULATION_TOTAL)
        ));

    }

    if (current_cycle > 1 && keep_recv) {

        randf_let_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "1" +
                seed_let +
                "002" +
                LIMIT +
                std::to_string(current_cycle)
        ));

        randf_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "1" +
                seed_inf +
                "002" +
                LIMIT +
                std::to_string(current_cycle)
        ));

        rng_pop_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "1" +
                seed_pop_inf +
                "0020100" +
                std::to_string(current_cycle)
        ));

    }

    if (!keep_recv) {

        std::cout << current_cycle << " STOP RECVING\n";

        rng_limit_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "0" +
                seed_lim +
                "0201000" +
                std::to_string(current_cycle)
        ));

        randf_sev_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "0" +
                seed_sev +
                "002" +
                std::to_string(current_cycle)
        ));

        randf_br_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "0" +
                seed_birth_rate +
                "002" +
                std::to_string(current_cycle)
        ));

        ceil_cure_thresh_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "0.00"
        ));

        randf_let_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "0" +
                seed_let +
                "002" +
                LIMIT +
                std::to_string(current_cycle)
        ));

        randf_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "0" +
                seed_inf +
                "002" +
                LIMIT +
                std::to_string(current_cycle)
        ));

        rng_pop_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "0" +
                seed_pop_inf +
                "0020100" +
                std::to_string(current_cycle)
        ));

    }

    std::cout << "------------------------------------------------\n";

    return false;

}
