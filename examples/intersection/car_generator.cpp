#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>

#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>

class car_generator : public SST::Component {

public:

    car_generator(SST::ComponentId_t, SST::Params &);

    bool tick(SST::Cycle_t);

    void finish() override;

    // Register the component
    SST_ELI_REGISTER_COMPONENT(
        car_generator, // class
        "intersection", // element library
        "car_generator", // component
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Car generator for the intersection",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    // Parameter name, description, default value
    SST_ELI_DOCUMENT_PARAMS(
        { "DELAY", "How long to wait between sending car events", "1s" },
        { "RANDOMSEED", "Random Seed for car type generation", "151515" }
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "is_car", "Port on which cars are sent", { "sst.Interfaces.StringEvent" }}
    )

private:
    SST::Output m_output;
    std::string clock;
    int64_t RandomSeed;
    SST::RNG::MarsagliaRNG *rng;
    SST::Link *is_car;
};


car_generator::car_generator(SST::ComponentId_t id, SST::Params &params) :
    SST::Component(id),
    clock(params.find<std::string>("DELAY", "1s")),
    RandomSeed(params.find<int64_t>("RANDOMSEED", 151515)),
    rng(new SST::RNG::MarsagliaRNG(11, RandomSeed)),
    is_car(configureLink("is_car")) {

    m_output.init("\033[33mcar_generator-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    // Get parameters
    m_output.verbose(CALL_INFO, 1, 0, "Minimum Delay Between Cars=%gs, Random Number Seed=%ld\n", std::stof(clock),
                   RandomSeed);

    // Register the clock
    registerClock(clock, new SST::Clock::Handler<car_generator>(this, &car_generator::tick));

    if (!is_car) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure port 'is_car'\n");
    }

}

void car_generator::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());

}

bool car_generator::tick(SST::Cycle_t) {
    // generating a random number between 0 and 1
    // 0 = No Car
    // 1 = Car
    int rndNumber = rng->generateNextInt32();
    rndNumber = (rndNumber & 0x0000FFFF) ^ ((rndNumber & 0xFFFF0000) >> 16);

    is_car->send(new SST::Interfaces::StringEvent(std::to_string(abs(rndNumber % 2))));
    return false;

}
