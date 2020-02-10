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
                "0020100" +
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
    }

    if (current_cycle == 1) {

        m_output.verbose(CALL_INFO, 1, 0, "----------------------------------------\n");
        m_output.verbose(CALL_INFO, 1, 0, "--------- SIMULATION INITIATED ---------\n");
        m_output.verbose(CALL_INFO, 1, 0, "----------------------------------------\n");
        m_output.verbose(CALL_INFO, 1, 0, "   Day  |  Cure  | Infected | Dead\n");
        m_output.verbose(CALL_INFO, 1, 0, "--------+--------+----------+---------\n");
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

//    if (current_cycle == LOOPEND - 4) {
//        std::cout << "RAM READING\n";
//
//        std::string addr = std::to_string(m_ram_addr::TOTAL_DEAD_ADDR);
//        align_signal_width('0', 8, addr);
//
//        ram_din_link->send(new SST::Interfaces::StringEvent(
//                std::to_string(m_keep_send) +
//                std::to_string(m_keep_recv) +
//                addr +
//                m_ram_read +
//                dont_care_data
//        ));
//    }

    if (!m_keep_recv) {

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
                "0020100" +
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

    TOTAL_INFECTED += TOTAL_INFECTED_TODAY;
    TOTAL_DEAD += TOTAL_DEAD_TODAY;

    if (CURE > 100.00 && m_loop_lock) {
        SIMTIME = current_cycle + 10;
        LOOPEND = (SIMTIME - 2);
        m_loop_lock = false;
    } else if (CURE <= 100.00) {
        m_output.verbose(CALL_INFO, 1, 0, " %6lu | %6.2f | %8d | %8d\n", current_cycle,
                         CURE, TOTAL_INFECTED_TODAY, TOTAL_DEAD_TODAY);
        std::string _cure = std::to_string((unsigned int) CURE);
        std::string _pop_inf = std::to_string(TOTAL_INFECTED_TODAY);
        std::string _pop_dead = std::to_string(TOTAL_DEAD_TODAY);
        std::cout << (unsigned int) CURE << ' ' << std::to_string((unsigned int) CURE) << '\n';
    }

    if (current_cycle == SIMTIME) {
        m_output.verbose(CALL_INFO, 1, 0, " Severity: %f\n", SEVERITY);
        m_output.verbose(CALL_INFO, 1, 0, " Infectivity: %f\n", INFECTIVITY);
        m_output.verbose(CALL_INFO, 1, 0, " Lethality: %f\n", LETHALITY);
        m_output.verbose(CALL_INFO, 1, 0, " Cure threshold: %d\n", CURE_THRESHOLD);
        m_output.verbose(CALL_INFO, 1, 0, " Total infected: %d\n", TOTAL_INFECTED);
        m_output.verbose(CALL_INFO, 1, 0, " Total dead: %d\n", TOTAL_DEAD);
        primaryComponentOKToEndSim();
    }
    return current_cycle == SIMTIME;

}
