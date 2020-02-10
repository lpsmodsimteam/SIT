#include "plague.hpp"
#include "setup.hpp"
#include "event_drivers.hpp"

bool plague::tick(SST::Cycle_t current_cycle) {

    m_keep_send = current_cycle < SIMTIME;
    m_keep_recv = current_cycle < SIMTIME - 1;
    m_cycle = current_cycle;

    if (TOTAL_INFECTED > CURE_THRESHOLD) {

        randf_rsrch_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "1" +
                seed_research +
                "002" +
                LIMIT +
                std::to_string(current_cycle)
        ));

        rng_mut_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "1" +
                seed_mutation +
                "0000008" +
                std::to_string(current_cycle)
        ));

        mutation_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                std::to_string(LETHALITY).back() +
                MUTATION
        ));

        CURE += RESEARCH;
        std::cout << current_cycle << " CURE " << CURE << " MUTATION " << MUTATION << " LETHALITY "
                  << std::to_string(LETHALITY).back() << ' ' << std::to_string(LETHALITY) << '\n';

    }

    if (current_cycle == 1) {

        rng_limit_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "1" +
                seed_lim +
                "0201000" +
                std::to_string(current_cycle)
        ));

    }

    if (current_cycle == LOOPBEGIN + 1) {

        ceil_cure_thresh_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                align_signal_width(8, SEVERITY * BIRTH_RATE * POPULATION_TOTAL)
        ));

    }

    if (current_cycle > 1 && m_keep_recv) {

        randf_let_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "1" +
                seed_let +
                "002" +
                LIMIT +
                std::to_string(current_cycle)
        ));

        randf_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "1" +
                seed_inf +
                "002" +
                LIMIT +
                std::to_string(current_cycle)
        ));

        rng_pop_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "1" +
                seed_pop_inf +
                "0020100" +
                std::to_string(current_cycle)
        ));

    }

    // if (current_cycle == LOOPEND - 4) {
    //     std::cout << "RAM WRITING\n";
    //     std::string total_dead = std::to_string(TOTAL_DEAD);
    //     align_signal_width('0', 8, total_dead);

    //     std::string addr = std::to_string(m_ram_addr::TOTAL_DEAD_ADDR);
    //     align_signal_width('0', 8, addr);

    //     ram_din_link->send(new SST::Interfaces::StringEvent(
    //             std::to_string(m_keep_send) +
    //             std::to_string(m_keep_recv) +
    //             addr +
    //             m_ram_write +
    //             total_dead
    //     ));
    // }

    if (current_cycle == LOOPEND - 4) {
        std::cout << "RAM READING\n";

        std::string addr = std::to_string(m_ram_addr::TOTAL_DEAD_ADDR);
        align_signal_width('0', 8, addr);

        ram_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                addr +
                m_ram_read +
                dont_care_data
        ));
    }

    if (!m_keep_recv) {

        std::cout << current_cycle << " STOP RECVING\n";

        // RAM COMPONENT
        std::string ram_cycle = std::to_string(current_cycle);
        align_signal_width('0', 8, ram_cycle);

        ram_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                dont_care_data +
                m_ram_noop +
                dont_care_data
        ));

        rng_limit_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "0" +
                seed_lim +
                "0201000" +
                std::to_string(current_cycle)
        ));

        randf_sev_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "0" +
                seed_sev +
                "002" +
                std::to_string(current_cycle)
        ));

        randf_br_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "0" +
                seed_birth_rate +
                "002" +
                std::to_string(current_cycle)
        ));

        ceil_cure_thresh_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "0.00"
        ));

        randf_let_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "0" +
                seed_let +
                "002" +
                LIMIT +
                std::to_string(current_cycle)
        ));

        randf_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "0" +
                seed_inf +
                "002" +
                LIMIT +
                std::to_string(current_cycle)
        ));

        randf_rsrch_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "0" +
                seed_research +
                "002" +
                LIMIT +
                std::to_string(current_cycle)
        ));

        rng_mut_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "0" +
                seed_mutation +
                "0020008" +
                std::to_string(current_cycle)
        ));

        rng_pop_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                "0" +
                seed_pop_inf +
                "0020100" +
                std::to_string(current_cycle)
        ));

    }

    std::cout << "------------------------------------------------\n";

    return CURE >= 100;

}
