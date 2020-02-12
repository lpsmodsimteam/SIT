#include "plague.hpp"
#include "setup.hpp"
#include "event_drivers.hpp"

bool plague::tick(SST::Cycle_t current_cycle) {

    m_keep_send = current_cycle < SIMTIME;
    m_keep_recv = current_cycle < SIMTIME - 1;
    m_cycle = current_cycle;
    std::string current_cycle_str = std::to_string(current_cycle);

    if (m_mem_read_flag) {

        std::string ram_addr = std::to_string(current_cycle % (SIMTIME / 2));
        align_signal_width('0', 8, ram_addr);
        flash_mem_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(m_keep_send) +
                std::to_string(m_keep_recv) +
                ram_addr +
                m_mem_read +
                ram_addr
        ));

    } else {

        if (m_total_infected > m_cure_threshold) {

            randf_rsrch_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    "1" +
                    seed_research +
                    "0020100" +
                    current_cycle_str
            ));

            rng_mut_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    "1" +
                    seed_mutation +
                    "0000008" +
                    current_cycle_str
            ));

            mutation_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    std::to_string(m_fatality).back() +
                            m_mutation
            ));

            m_cure += m_research;
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
                    current_cycle_str
            ));

        }

        if (current_cycle == LOOPBEGIN + 1) {

            ceil_cure_thresh_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    align_signal_width(8, m_severity * m_birth_rate * POPULATION_TOTAL)
            ));

        }

        if (current_cycle > 1 && m_keep_recv) {

            randf_fat_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    "1" +
                    seed_let +
                    "002" +
                    m_limit +
                    current_cycle_str
            ));

            randf_inf_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    "1" +
                    seed_inf +
                    "002" +
                    m_limit +
                    current_cycle_str
            ));

            rng_pop_inf_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    "1" +
                    seed_pop_inf +
                    "0020100" +
                    current_cycle_str
            ));

        }

        if (!m_keep_send) {

            std::cout << current_cycle << " Reading Memory...\n";
            m_mem_read_flag = true;
            SIMTIME = current_cycle * 2;
            m_fp = std::fopen("memory_dump.txt", "w");

        }

        if (!m_keep_recv) {

            rng_limit_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    "0" +
                    seed_lim +
                    "0201000" +
                    current_cycle_str
            ));

            randf_sev_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    "0" +
                    seed_sev +
                    "002" +
                    current_cycle_str
            ));

            randf_br_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    "0" +
                    seed_birth_rate +
                    "002" +
                    current_cycle_str
            ));

            ceil_cure_thresh_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    "0.00"
            ));

            randf_fat_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    "0" +
                    seed_let +
                    "002" +
                    m_limit +
                    current_cycle_str
            ));

            randf_inf_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    "0" +
                    seed_inf +
                    "002" +
                    m_limit +
                    current_cycle_str
            ));

            randf_rsrch_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    "0" +
                    seed_research +
                    "0020100" +
                    current_cycle_str
            ));

            rng_mut_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    "0" +
                    seed_mutation +
                    "0020008" +
                    current_cycle_str
            ));

            rng_pop_inf_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    "0" +
                    seed_pop_inf +
                    "0020100" +
                    current_cycle_str
            ));

        }

        m_total_infected += m_total_infected_today;
        m_total_dead += m_total_dead_today;

        if (m_cure > 100.00 && m_loop_lock) {

            SIMTIME = current_cycle + 5;
            LOOPEND = (SIMTIME - 2);
            m_loop_lock = false;

        } else if (m_cure <= 100.00) {

            m_output.verbose(CALL_INFO, 1, 0, " %6lu | %6.2f | %8d | %8d\n", current_cycle,
                             m_cure, m_total_infected_today, m_total_dead_today);

            std::string _pop_inf = std::to_string(m_total_infected_today);
            std::string _pop_dead = std::to_string(m_total_dead_today);
            std::string _cure = std::to_string((unsigned int) m_cure);
            align_signal_width('0', 3, _pop_inf);
            align_signal_width('0', 3, _pop_dead);
            align_signal_width('0', 2, _cure);
            std::string ram_data = _pop_inf + _pop_dead + _cure;
            std::string ram_addr = current_cycle_str;
            align_signal_width('0', 8, ram_addr);

            flash_mem_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(m_keep_send) +
                    std::to_string(m_keep_recv) +
                    ram_addr +
                    m_mem_write +
                    ram_data
            ));

        }

    }  // m_mem_read_flag

    if (current_cycle == SIMTIME) {
        m_output.verbose(CALL_INFO, 1, 0, "Severity: %f\n", m_severity);
        m_output.verbose(CALL_INFO, 1, 0, "Infectivity: %f\n", m_infectivity);
        m_output.verbose(CALL_INFO, 1, 0, "Fatality: %f\n", m_fatality);
        m_output.verbose(CALL_INFO, 1, 0, "Birth rate: %f\n", m_birth_rate);
        m_output.verbose(CALL_INFO, 1, 0, "Cure threshold: %d\n", m_cure_threshold);
        m_output.verbose(CALL_INFO, 1, 0, "Total infected: %d\n", m_total_infected);
        m_output.verbose(CALL_INFO, 1, 0, "Total dead: %d\n", m_total_dead);
        primaryComponentOKToEndSim();
    }

    return current_cycle == SIMTIME;

}
