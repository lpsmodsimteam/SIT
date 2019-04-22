/**
Main logic for the intersection

Connects to 2 traffic lights (PyRTL wrapped in SST) and 2 car generators
Keeps track of some statistics for number of cars and largest backups at the lights
*/

#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/elementinfo.h>

class intersection : public SST::Component {

public:

    intersection(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    void handle_light0(SST::Event *);

    void handle_light1(SST::Event *);

    void handle_cars0(SST::Event *);

    void handle_cars1(SST::Event *);

    SST_ELI_REGISTER_COMPONENT(
        intersection,
        "intersection",
        "intersection",
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Main Intersection for traffic simulation",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        { "clock", "Clock frequency or period", "1Hz" },
        { "simDuration", "How long to run the simulation (ticks)", "200" }
    )

    SST_ELI_DOCUMENT_PORTS(
        { "light0", "Port for connection to first traffic light", { "sst.Interfaces.SST::Interfaces::StringEvent" }},
        { "light1", "Port for connection to second traffic light", { "sst.Interfaces.SST::Interfaces::StringEvent" }},
        { "cars0", "Port on which cars are received", { "sst.Interfaces.SST::Interfaces::StringEvent" }},
        { "cars1", "Port on which cars are received", { "sst.Interfaces.SST::Interfaces::StringEvent" }}
    )

private:
    SST::Output output;
    std::string clock;
    SST::Cycle_t simDuration;

    SST::Link *light0;
    SST::Link *light1;
    SST::Link *cars0;
    SST::Link *cars1;

    int road0;
    int road1;
    int totalCars0;
    int totalCars1;
    int backup0;
    int backup1;
    int numtics;
};

intersection::intersection(SST::ComponentId_t id, SST::Params &params) :
    SST::Component(id),
    // Collect parameters
    clock(params.find<std::string>("clock", "1Hz")),
    simDuration(params.find<SST::Cycle_t>("simDuration", 200)),
    light0(configureLink(
        "light0",
        new SST::Event::Handler<intersection>(this, &intersection::handle_light0))
    ),
    light1(configureLink(
        "light1",
        new SST::Event::Handler<intersection>(this, &intersection::handle_light1))
    ),
    cars0(configureLink(
        "cars0",
        new SST::Event::Handler<intersection>(this, &intersection::handle_cars0))
    ),
    cars1(configureLink(
        "cars1",
        new SST::Event::Handler<intersection>(this, &intersection::handle_cars1))
    ) {

    output.init("intersection-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

    // Error check params
    if (simDuration <= 0) {
        output.fatal(CALL_INFO, -1, "Error: simDuration must be greater than zero.\n");
    }
    output.verbose(CALL_INFO, 1, 0, "simDuration=%d Hours\n", ((int) simDuration / 3600));

    // Just register a plain clock for this simple example
    registerClock(clock, new SST::Clock::Handler<intersection>(this, &intersection::tick));

    // Configure our ports
    if (!(light0 && light1 && cars0 && cars1)) {
        output.fatal(CALL_INFO, -1, "Failed to configure port\n");
    }

    // Tell SST to wait until we authorize it to exit
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();
}

void intersection::setup() {
    road0 = 0;
    road1 = 0;
    totalCars0 = 0;
    totalCars1 = 0;
    backup0 = 0;
    backup1 = 0;
    numtics = 0;
}

void intersection::finish() {
    printf("\nCars that drove through Traffic Light 0: %d\n", totalCars0);
    printf("Cars that drove through Traffic Light 1: %d\n", totalCars1);
    printf("Largest backup at Traffic Light 0: %d\n", backup0);
    printf("Largest backup at Traffic Light 1: %d\n\n", backup1);
}

// Exit when enough clock ticks have happened
bool intersection::tick(SST::Cycle_t current_cycle) {
    if (current_cycle == 1) {
        printf("\n Hour | Total Cars TL0 | Total Cars TL1\n");
        printf("------+----------------+---------------\n");
    }
    numtics++;
    if (!(numtics % 3600)) {
        printf(" %4d | %14d | %14d\n", numtics / 3600, totalCars0, totalCars1);
    }
    if (current_cycle >= simDuration) {
        primaryComponentOKToEndSim();
        return true;
    } else {
        return false;
    }
}

// If the light is green or yellow, allow cars to go through one at a time
void intersection::handle_light0(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se) {
        if (se->getString().c_str()[0] == 'g' || se->getString().c_str()[0] == 'y') {
            if (road0 > 0) {
                road0--;
            }
        }
    }

    delete ev;

}

// If the light is green or yellow, allow cars to go through one at a time
void intersection::handle_light1(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se) {
        if (se->getString().c_str()[0] == 'g' || se->getString().c_str()[0] == 'y') {
            if (road1 > 0) {
                road1--;
            }
        }
    }

    delete ev;

}

// Add cars to the road when they arrive and keep track of the largest backup
void intersection::handle_cars0(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se) {

        if (se->getString().c_str()[0] == '1') {
            road0++;
            totalCars0++;
            if (road0 > backup0) {
                backup0 = road0;
            }
        }

    }

    delete ev;

}

// Add cars to the road when they arrive and keep track of the largest backup
void intersection::handle_cars1(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se) {

        if (se->getString().c_str()[0] == '1') {
            road1++;
            totalCars1++;
            if (road1 > backup1) {
                backup1 = road1;
            }
        }

    }

    delete ev;

}
