/**
Car Wash simulation

There is a separate component which generates "cars" as an input
These cars are then queued up and run through the carwash
using subcomponent "bays"
The time it took for each car to go through is recorded to keep track
of satisfied customers.
*/

#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include "car_wash.hpp"

car_wash::car_wash(SST::ComponentId_t id, SST::Params &params) : SST::Component(id) {

    output.init("car_wash-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

    // Get parameters
    runTime = params.find<int64_t>("simulationTime", 24);
    clock = params.find<std::string>("tickFreq", "60s");

    // Figure out the ticks per hour using the given tickFreq
    if (clock.compare(clock.length() - 1, 1, "s") == 0) {
        ticksPerHour = (60.0 / std::stof(clock)) * 60.0;
    } else if (clock.compare(clock.length() - 2, 2, "Hz") == 0) {
        ticksPerHour = std::stof(clock) * 2;
    }

    // Initialize subcomponents
    // Using this technique allows for 1 or more bays
    SST::SubComponentSlotInfo *info = getSubComponentSlotInfo("bay");
    if (!info) {
        output.fatal(CALL_INFO, -1, "Must specify at least one bay.\n");
    } else {
        info->createAll(subComps);
    }

    // tell the simulator not to end without us
    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();

    // Set our Main Clock (register main clocks)
    // This will automatically be called by the SST framework
    registerClock(clock, new SST::Clock::Handler<car_wash>(this, &car_wash::tick));

    // Setup statistics
    smallCarsWashed = registerStatistic<int>("smallCarsWashed");
    largeCarsWashed = registerStatistic<int>("largeCarsWashed");
    noCarEvents = registerStatistic<int>("noCarEvents");
    smallCarsWaiting = registerStatistic<int>("smallCarsWaiting");
    largeCarsWaiting = registerStatistic<int>("largeCarsWaiting");

    // Configure our car_type
    car_type = configureLink("car_type", new SST::Event::Handler<car_wash>(this, &car_wash::handleEvent));
    if (!car_type) {
        output.fatal(CALL_INFO, -1, "Failed to configure car_type 'car_type'\n");
    }

}

// Initialize all of the simulation variables and parameters
void car_wash::setup() {

    ptrCarRecordList = nullptr;
    CarWash.currentTime = 0;
    CarWash.smallCarsWashed = 0;
    CarWash.largeCarsWashed = 0;
    CarWashTick = 0;
    HourlyReport.smallCarsArrived = 0;
    HourlyReport.largeCarsArrived = 0;
    HourlyReport.noCarsArrived = 0;
    CarType = NO_CAR;

    std::cout << std::endl;
    std::cout << "Welcome to Uncle Ed's Fantastic, Awesome, and Incredible Carwash!" << std::endl;
    std::cout << "Simulation time will be: " << runTime << " hours" << std::endl;
    std::cout << std::endl;

}


// Print out statistics at the end
void car_wash::finish() {

    CAR_RECORD *ptr = ptrCarRecordList;
    int SmallCarCustomers = 0;
    int LargeCarCustomers = 0;

    std::cout << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;
    std::cout << "Uncle Ed's Carwash Simulation has completed!" << std::endl;
    std::cout << "Here's a summary of the results:" << std::endl;
    std::cout << "Simulation duration was: " << runTime << " hours of operation." << std::endl;
    std::cout << "Small Cars Washed: " << CarWash.smallCarsWashed << std::endl;
    std::cout << "Large Cars Washed: " << CarWash.largeCarsWashed << std::endl;
    std::cout << "Total Cars washed: " << (CarWash.smallCarsWashed + CarWash.largeCarsWashed) << std::endl;

    for (;;) {
        if (ptr) {
            if (SMALL_CAR == ptr->CarSize) {
                SmallCarCustomers++;
            } else if (LARGE_CAR == ptr->CarSize) {
                LargeCarCustomers++;
            }
            ptr = ptr->ptrNext;
        } else {
            std::cout << std::endl;
            break;
        }
    }

    std::cout << "Customers that are still waiting in line" << std::endl;
    std::cout << "Small Cars: " << SmallCarCustomers << std::endl;
    std::cout << "Large Cars: " << LargeCarCustomers << std::endl;
    std::cout << "Total: " << SmallCarCustomers + LargeCarCustomers << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;
}


// Receive events that contain the CarType, add the cars to the queue
void car_wash::handleEvent(SST::Event *ev) {
    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {
        CarType = std::stoi(&(se->getString().c_str()[0]));
        if (NO_CAR == CarType) {
            HourlyReport.noCarsArrived++;
            noCarEvents->addData(1);
        } else {
            QueueACar(CarType);
            if (SMALL_CAR == CarType) {
                HourlyReport.smallCarsArrived++;
                smallCarsWaiting->addData(1);
            } else if (LARGE_CAR == CarType) {
                HourlyReport.largeCarsArrived++;
                largeCarsWaiting->addData(1);
            }
        }
    }
    delete ev;
}


// Runs every clock tick
bool car_wash::tick(SST::Cycle_t) {

    CarWash.currentTime = CarWashTick;

    // Announce the top of the hour and give a little summary of where we are at with our carwashing business
    if ((CarWashTick > 0) && (!(CarWashTick % static_cast<int>(ticksPerHour)))) {
        std::cout << "------------------------------------------------------------------" << std::endl;
        std::cout << "Time = " << CarWashTick / ticksPerHour << " another simulated hour has passed! Summary:"
                  << std::endl;
        std::cout << "  Small Cars = " << HourlyReport.smallCarsArrived << std::endl;
        std::cout << "  Large Cars = " << HourlyReport.largeCarsArrived << std::endl;
        std::cout << "  No Cars = " << HourlyReport.noCarsArrived << std::endl;
        std::cout << std::endl;
        HourlyReport.smallCarsArrived = 0;
        HourlyReport.largeCarsArrived = 0;
        HourlyReport.noCarsArrived = 0;
    }

    // Car Wash simulation has finished
    if ((runTime * ticksPerHour) <= CarWash.currentTime) {
        primaryComponentOKToEndSim();
        return (true);
    }

    // See what carwash bays can be cycled, and if any new cars can be processed in
    CarWashClockTick();
    CarWashTick++;
    return false;
}


// Add a car to the queue
void car_wash::QueueACar(int carSize) {
    // Find the end of the car queue
    CAR_RECORD *ptr = ptrCarRecordList;

    if (ptr) { // Check to see if the list is empty
        while (ptr->ptrNext) { // If not walk down the list to the end
            ptr = ptr->ptrNext;
        }
    }
    // Allocate a bit of memory, formatted for a car record, and set the pointers.
    if (!ptr) { // First entry is a special case
        if (!(ptrCarRecordList = new CAR_RECORD)) {
            printf("Error allocating memory for the first new car wash record\n");
        }
        ptr = ptrCarRecordList; // Anchor the new list
    } else {
        if (!(ptr->ptrNext = new CAR_RECORD)) {
            printf("Error allocating memory to create a new car wash record\n");
        }
        ptr = ptr->ptrNext;    // Index to that pointer
    }

    ptr->ptrNext = nullptr; // Set the pointer
    ptr->CarSize = carSize; // Set the car's size
}


// On every clock tick this checks to see if cars can be put into an empty bay
void car_wash::CarWashClockTick() {
    CAR_RECORD *pTemp;
    int numEmpty = 0;
    // Now let's see if we have a bay empty, or ready to be emptied
    // Return of true implies the bay is empty and ready for a new car

    // Run tick on all the bays and count the number of empty bays
    for (auto sub : subComps) {
        sub->tick();
        if (sub->isOccupied() == WASH_BAY_EMPTY) {
            numEmpty++;
        }
    }

    // Now that we've updated the bays, let's fill any that have emptied.
    // Are there any cars waiting?
    while ((numEmpty > 0) && ptrCarRecordList) {
        bool progress = false;
        // Check all they bays for an empty bay
        for (auto sub : subComps) {
            if (sub->isOccupied() == WASH_BAY_EMPTY) {
                // if the bay size matches the car use it
                if (sub->baySize() == ptrCarRecordList->CarSize) {
                    sub->newCar(ptrCarRecordList->CarSize);
                    pTemp = ptrCarRecordList;
                    ptrCarRecordList = ptrCarRecordList->ptrNext;
                    delete pTemp;
                    numEmpty--;
                    progress = true;
                    if (sub->baySize() == LARGE_CAR) {
                        CarWash.largeCarsWashed++;
                        largeCarsWaiting->addData(-1);
                        largeCarsWashed->addData(1);
                    } else {
                        CarWash.smallCarsWashed++;
                        smallCarsWaiting->addData(-1);
                        smallCarsWashed->addData(1);
                    }
                    break;
                }
            }
        }
        // If we couldn't match a car to a bay try putting the car in a large bay
        if (!progress) {
            for (auto sub : subComps) {
                if (sub->isOccupied() == WASH_BAY_EMPTY) {
                    if (sub->baySize() == LARGE_CAR) {
                        sub->newCar(ptrCarRecordList->CarSize);
                        pTemp = ptrCarRecordList;
                        ptrCarRecordList = ptrCarRecordList->ptrNext;
                        delete pTemp;
                        numEmpty--;
                        progress = true;
                        CarWash.smallCarsWashed++;
                        smallCarsWaiting->addData(-1);
                        smallCarsWashed->addData(1);
                        break;
                    }
                }
            }
        }
        // If we couldn't put the car in any bay we have to wait
        if (!progress) {
            break;
        }
    }
}
