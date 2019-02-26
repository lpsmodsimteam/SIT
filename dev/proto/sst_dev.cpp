/**
Simple 4-bit Up-Counter Model with one clock
*/

#include "sst_dev.h"
#include <sst/core/sst_config.h>

#define PROC_STR "proc"
#define RANK_STR "rank"
#define PID_STR "pid"

// Component Constructor
sst_dev::sst_dev(SST::ComponentId_t id, SST::Params &params) : SST::Component(id) {

    init_MPI();

    // Find out rank, size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Initialize output
    m_output.init("\033[32mmodule-" + getName() + "\033[0m (pid: " + std::to_string(getpid()) + ") -> ", 1, 0,
                  SST::Output::STDOUT);

    m_num_procs = params.find<int>("num_procs", 1);
    params.find_array<std::string>("mods", m_modules);

    for (int i = 0; i < m_modules.size(); i++) {
        if (!i) {
            m_modules[i] = m_modules[i].substr(1, m_modules[i].size() - 2);
        } else {
            m_modules[i] = m_modules[i].substr(2, m_modules[i].size() - 3);
        }
        std::cout << m_modules[i] << std::endl;
    }

    // Just register a plain clock for this simple example
    registerClock("500MHz", new SST::Clock::Handler<sst_dev>(this, &sst_dev::tick));

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

}

sst_dev::~sst_dev() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying master...\n");
    MPI_Comm_free(&m_inter_com);

    m_data_out.clear();
    m_procs.clear();

    delete[] send_buf;
    delete[] recv_buf;

}

// setup is called by SST after a component has been constructed and should be used
// for initialization of variables
void sst_dev::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    std::cout << "Master pid: " << getpid() << std::endl;

    std::vector<char*> PROCS;
    PROCS.reserve(m_modules.size());

    for(size_t i = 0; i < m_modules.size(); ++i) {
        PROCS.push_back(&m_modules[i][0u]);
    }

    auto MAX_PROCS = new int[m_num_procs];
    auto ERR_CODES = new int[m_num_procs];
    auto INFOS = new MPI_Info[m_num_procs];

    for (int i = 0; i < m_num_procs; i++) {
        MAX_PROCS[i] = 1;
        INFOS[i] = MPI_INFO_NULL;
    }

    send_buf = new char[m_num_procs][BUFSIZE];
    recv_buf = new char[m_num_procs][BUFSIZE];

    MPI_Comm_spawn_multiple(m_num_procs, &PROCS[0u], MPI_ARGVS_NULL, MAX_PROCS, INFOS,
                            0, MPI_COMM_SELF, &m_inter_com, ERR_CODES);

    int child_pids[m_num_procs];
    int child_ranks[m_num_procs];

    // gather PIDs of spawned processes
    MPI_Gather(nullptr, 1, MPI_INT, child_pids, 1, MPI_INT, MPI_ROOT, m_inter_com);
    // gather ranks of spawned processes
    MPI_Gather(nullptr, 1, MPI_INT, child_ranks, 1, MPI_INT, MPI_ROOT, m_inter_com);

    // construct a summary variable with PIDs, ranks and process names
    for (int i = 0; i < m_num_procs; i++) {

        m_procs[i] = {
                {PROC_STR, PROCS[i]},
                {RANK_STR, child_ranks[i]},
                {PID_STR,  child_pids[i]}
        };
        m_data_out[i] = json{};  // initialize empty JSON objects

    }

    PROCS.clear();
    delete[] MAX_PROCS;
    delete[] ERR_CODES;
    // delete[] PROCS;
    delete[] INFOS;

    std::cout << m_procs.dump(4) << std::endl;

}

// finish is called by SST before the simulation is ended and should be used
// to clean up variables and memory
void sst_dev::finish() {
    m_output.verbose(CALL_INFO, 1, 0, "Component is being finished.\n");
}

// clockTick is called by SST from the registerClock function
// this function runs once every clock cycle
bool sst_dev::tick(SST::Cycle_t current_cycle) {

    std::cout << "<----------------------------------------------------" << std::endl;

    int destroyed_mods = 0;

    for (int proc = 0; proc < m_num_procs; proc++) {

        // assign SST clock to SystemC clock
        m_data_out[proc]["clock"] = current_cycle;

        // 4-bit logical right shifter
        if (proc == 0) {

            m_data_out[proc]["on"] = true;
            m_data_out[proc]["reset"] = true;

            // turn module off at 52 ns
            if (current_cycle >= 3) {
                if (current_cycle == 3) {
                    std::cout << "RESET OFF" << std::endl;
                }
                m_data_out[proc]["reset"] = false;
            }

            // turn module off at 52 ns
            if (current_cycle >= 38) {
                if (current_cycle == 38) {
                    std::cout << "FIBONACCI LFSR MODULE OFF" << std::endl;
                }
                m_data_out[proc]["on"] = false;
            }

            // Galois LFSR
        } else if (proc == 1) {

            m_data_out[proc]["on"] = true;
            m_data_out[proc]["reset"] = true;

            // turn module off at 52 ns
            if (current_cycle >= 3) {
                if (current_cycle == 3) {
                    std::cout << "RESET OFF" << std::endl;
                }
                m_data_out[proc]["reset"] = false;
            }

            // turn module off at 52 ns
            if (current_cycle >= 38) {
                if (current_cycle == 38) {
                    std::cout << "GALOIS LFSR MODULE OFF" << std::endl;
                }
                m_data_out[proc]["on"] = false;
            }

        }

        snprintf(send_buf[proc], sizeof(send_buf[proc]), "%s",
                 m_data_out[proc].dump().c_str());
        if (!m_data_out[proc]["on"]) {
            destroyed_mods++;
        }

    }

    transmit_signals(*send_buf, m_inter_com);
    if (destroyed_mods != m_num_procs) {
        receive_signals(*recv_buf, m_inter_com);
        // fib_lfsr_out = json::parse(recv_buf[0])["fib_lfsr_out"].get<int>();
        // galois_lfsr_out = json::parse(recv_buf[1])["galois_lfsr"].get<int>();
        m_output.verbose(CALL_INFO, 1, 0, "{%s, %s}\n", recv_buf[0], recv_buf[1]);
    }
    std::cout << "---------------------------------------------------->" << std::endl;

    return false;

}