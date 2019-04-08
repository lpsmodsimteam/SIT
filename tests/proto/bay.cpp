/**
Car Wash bay

This subcomponent is a bay that washes cars for the Car Wash
The bay can be a small bay or a large bay
Small bays can only wash small cars
Large bays can wash small or large cars
*/

#include <sst/core/sst_config.h>

#include "car_wash.hpp"

class bay : public carWashBay {

public:

    bay(SST::Component *owningComponent, SST::Params &params);

    bool newCar(int) override;

    int baySize() override;

    int isOccupied() override;

    void tick() override;

    // Register the subcomponent
    SST_ELI_REGISTER_SUBCOMPONENT(
        bay, // class
        "proto", // element library
        "bay", // subcomponent
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Bay subcomponent for the carwash",
        "SST::car_wash::carWashBay" // subcomponent slot
    )

    // Parameter name, description, default value
    SST_ELI_DOCUMENT_PARAMS(
        { "size", "Size of the bay (1 = small, 2 = large)", "1" },
        { "smallCarWashTime", "How long to wash a small car (ticks)", "3" },
        { "largeCarWashTime", "How long to wash a large car (ticks)", "5" }
    )

    // Statistic name, description, unit, enable level
    SST_ELI_DOCUMENT_STATISTICS(
        { "carsWashed", "Number of cars washed", "cars", 1 },
        { "idleTime", "Time spent not washing cars", "ticks", 1 }
    )


private:
    SST::Output output;
    int bay_size;
    int smallTime;
    int largeTime;
    int occupied;
    int timeToOccupy;
    Statistic<uint32_t> *carsWashed;
    Statistic<uint32_t> *idleTime;
};


bay::bay(SST::Component *owningComponent, SST::Params &params) : carWashBay(owningComponent) {

    output.init("bay-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

    // Get parameters
    bay_size = params.find<int>("size", 1);
    smallTime = params.find<int>("smallCarWashTime", 3);
    largeTime = params.find<int>("largeCarWashTime", 5);

    // Setup statistics
    carsWashed = registerStatistic<uint32_t>("carsWashed");
    idleTime = registerStatistic<uint32_t>("idleTime");

    occupied = WASH_BAY_EMPTY;
}

// Enters a new car into the bay if empty
bool bay::newCar(int CarType) {

    if (occupied == WASH_BAY_EMPTY) {
        if (CarType == LARGE_CAR) {
            if (bay_size == LARGE_CAR) {
                timeToOccupy = largeTime;
                occupied = WASH_BAY_FULL;
                return true;
            }
        } else {
            timeToOccupy = smallTime;
            occupied = WASH_BAY_FULL;
            return true;
        }
    }
    return false;
}

// Return the bay size
int bay::baySize() {
    return bay_size;
}

// Return occupied status of the bay
int bay::isOccupied() {
    return occupied;
}

// Count down time on any cars in bay
void bay::tick() {
    if (occupied != WASH_BAY_EMPTY) {
        timeToOccupy--;
        if (timeToOccupy <= 0) {
            occupied = WASH_BAY_EMPTY;
            carsWashed->addData(1);
        }
    } else {
        idleTime->addData(1);
    }
}

