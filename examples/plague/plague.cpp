/**
Main logic for the plague

Connects to 2 traffic lights and 2 car generators
Keeps track of some statistics for number of cars and largest backups at the lights
*/

#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>

#include <sst/core/component.h>
#include <sst/core/link.h>

class plague : public SST::Component {

public:

    plague(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    void handle_light0(SST::Event *);

    void handle_light1(SST::Event *);

    void handle_cars0(SST::Event *);

    void handle_cars1(SST::Event *);

    SST_ELI_REGISTER_COMPONENT(
        plague,
        "plague",
        "plague",
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Main plague driver",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        { "CLOCK", "Clock frequency or period", "1Hz" },
        { "SIM_DURATION", "How long to run the simulation (ticks)", "200" }
    )

    SST_ELI_DOCUMENT_PORTS(
        { "light0", "Port for connection to first traffic light", { "sst.Interfaces.SST::Interfaces::StringEvent" }},
        { "light1", "Port for connection to second traffic light", { "sst.Interfaces.SST::Interfaces::StringEvent" }},
        { "cars0", "Port on which cars are received", { "sst.Interfaces.SST::Interfaces::StringEvent" }},
        { "cars1", "Port on which cars are received", { "sst.Interfaces.SST::Interfaces::StringEvent" }}
    )

private:
    SST::Output m_output;
    SST::Link *light[2], *cars[2];
    std::string clock, seed;

    int POPULATION_TOTAL;
    int POPULATION_HEALTHY;
    int POPULATION_INFECTED;
    int TOTAL_INFECTED;
    int POPULATION_DEAD;
    int TOTAL_DEAD;
    int POPULATION_AFFECTED;

    double BIRTH_RATE;
    double SEVERITY;
    double LETHALITY;
    double INFECTIVITY;
    double CURE;
    int CURE_THRESHOLD;
    int TIME;
};

plague::plague(SST::ComponentId_t id, SST::Params &params) :
    SST::Component(id),
    clock(params.find<std::string>("CLOCK", "1Hz")),
    seed(params.find<std::string>("SEED", "0000")),
    light{
        configureLink(
            "light0", new SST::Event::Handler<plague>(this, &plague::handle_light0)
        ),
        configureLink(
            "light1", new SST::Event::Handler<plague>(this, &plague::handle_light1)
        )
    },
    cars{
        configureLink(
            "cars0", new SST::Event::Handler<plague>(this, &plague::handle_cars0)
        ),
        configureLink(
            "cars1", new SST::Event::Handler<plague>(this, &plague::handle_cars1)
        )
    },
    road{0, 0}, total_cars{0, 0}, backup{0, 0}, n_ticks(0) {

    m_output.init("\033[34mplague-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    // Just register a plain clock for this simple example
    registerClock(clock, new SST::Clock::Handler<plague>(this, &plague::tick));

    // Configure our ports
    if (!(light[0] && light[1] && cars[0] && cars[1])) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure is_car\n");
    }

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();
}

void plague::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

}

void plague::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "\n");
    m_output.verbose(CALL_INFO, 1, 0, "-------------------------------------------\n");
    m_output.verbose(CALL_INFO, 1, 0, "---------- SIMULATION STATISTICS ----------\n");
    m_output.verbose(CALL_INFO, 1, 0, "-------------------------------------------\n");
    m_output.verbose(CALL_INFO, 1, 0, "Traffic Light | Total Cars | Largest Backup\n");
    m_output.verbose(CALL_INFO, 1, 0, "--------------+------------+---------------\n");
    m_output.verbose(CALL_INFO, 1, 0, "            0 | %10d | %13d\n", total_cars[0], backup[0]);
    m_output.verbose(CALL_INFO, 1, 0, "            1 | %10d | %13d\n", total_cars[1], backup[1]);
    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());

}

// If the light is green or yellow, allow cars to go through one at a time
void plague::handle_light0(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se) {
        if (se->getString().c_str()[0] == 'g' || se->getString().c_str()[0] == 'y') {
            if (road[0] > 0) {
                road[0]--;
            }
        }
    }

    // delete ev;

}

// If the light is green or yellow, allow cars to go through one at a time
void plague::handle_light1(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);



    if (se) {
        if (se->getString().c_str()[0] == 'g' || se->getString().c_str()[0] == 'y') {
            if (road[1] > 0) {
                road[1]--;
            }
        }
    }

    // delete ev;

}

// Add cars to the road when they arrive and keep track of the largest backup
void plague::handle_cars0(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se) {

        if (se->getString().c_str()[0] == '1') {
            road[0]++;
            total_cars[0]++;
            if (road[0] > backup[0]) {
                backup[0] = road[0];
            }
        }

    }

    delete ev;

}

// Add cars to the road when they arrive and keep track of the largest backup
void plague::handle_cars1(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se) {

        if (se->getString().c_str()[0] == '1') {
            road[1]++;
            total_cars[1]++;
            if (road[1] > backup[1]) {
                backup[1] = road[1];
            }
        }

    }

    delete ev;

}

// Exit when enough clock ticks have happened
bool plague::tick(SST::Cycle_t current_cycle) {

    if (current_cycle == 1) {
        m_output.verbose(CALL_INFO, 1, 0, "--------------------------------------\n");
        m_output.verbose(CALL_INFO, 1, 0, "-------- SIMULATION INITIATED --------\n");
        m_output.verbose(CALL_INFO, 1, 0, "--------------------------------------\n");
        m_output.verbose(CALL_INFO, 1, 0, "Hour | Total Cars TL0 | Total Cars TL1\n");
        m_output.verbose(CALL_INFO, 1, 0, "-----+----------------+---------------\n");
    }

    n_ticks++;

    if (!(n_ticks % 3600)) {
        m_output.verbose(CALL_INFO, 1, 0, " %3d | %14d | %14d\n", n_ticks / 3600, total_cars[0], total_cars[1]);
    }

    return false;

}
