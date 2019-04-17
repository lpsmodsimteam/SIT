/**
Simple Model with one clock that functions as a wrapper for a PyRTL simulation

Uses named pipes to send and receive data between SST and PyRTL
*/

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sst/core/sst_config.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/component.h>
#include <sst/core/link.h>
#include <sst/core/elementinfo.h>

class traffic_light : public SST::Component {

public:

    traffic_light(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    SST_ELI_REGISTER_COMPONENT(
        traffic_light,
        "intersection",
        "traffic_light",
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Demonstration of a PyRTL hardware simulation in SST",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        { "clock", "Clock frequency or period", "1Hz" },
        { "greenTime", "How long the green light stays on", "30" },
        { "yellowTime", "How long the yellow light stays on", "3" },
        { "redTime", "How long the red light stays on", "33" },
        { "startGreen", "Start the light as Green (0 -> start on Red)", "0" },
        { "inputPipe", "Name of the input pipe", "/tmp/output" },
        { "outputPipe", "Name of the output pipe", "/tmp/input" }
    )

    SST_ELI_DOCUMENT_PORTS(
        { "port", "Port on which to send/recv messages", { "sst.Interfaces.StringEvent" }}
    )

private:
    SST::Output output;
    SST::Link *port;

    int inFifo{};
    int outFifo{};

    std::string clock;
    std::string inputPipe;
    std::string outputPipe;
    int greenTime;
    int yellowTime;
    int redTime;
    int startGreen;
    char s[9]{};
};

traffic_light::traffic_light(SST::ComponentId_t id, SST::Params &params) :
    SST::Component(id),
    // Collect all the parameters from the project driver
    clock(params.find<std::string>("clock", "1Hz")),
    greenTime(params.find<int>("greenTime", 30)),
    yellowTime(params.find<int>("yellowTime", 3)),
    redTime(params.find<int>("redTime", 33)),
    startGreen(params.find<int>("startGreen", 0)),
    inputPipe(params.find<std::string>("inputPipe", "/tmp/output")),
    outputPipe(params.find<std::string>("outputPipe", "/tmp/input")),
    port(configureLink("port")) {

    output.init("traffic_light-" + getName() + "-> ", 1, 0, SST::Output::STDOUT);

    // Check parameters
    if (!(greenTime && yellowTime && redTime)) {
        output.fatal(CALL_INFO, -1, "Error: times must be greater than zero.\n");
    }

    // Print parameters
    output.verbose(CALL_INFO, 1, 0, "greenTime=%d, yellowTime=%d, redTime=%d, startGreen=%d\n",
                   greenTime, yellowTime, redTime, startGreen);

    // Just register a plain clock for this simple example
    registerClock(clock, new SST::Clock::Handler<traffic_light>(this, &traffic_light::tick));

    // Configure our port
    if (!port) {
        output.fatal(CALL_INFO, -1, "Failed to configure port 'port'\n");
    }

}

void traffic_light::setup() {

    // Connect to the named pipes when they are available
    while (access(inputPipe.c_str(), R_OK)) {}
    inFifo = open(inputPipe.c_str(), O_RDONLY);
    while (access(outputPipe.c_str(), W_OK)) {}
    outFifo = open(outputPipe.c_str(), O_WRONLY);

    // Load the initial times into the PyRTL stopLight
    sprintf(s, "1%x%02x%02x%02x", startGreen, greenTime, yellowTime, redTime);
    write(outFifo, s, 8);
    char r[2] = "\0";
    read(inFifo, r, 1);
    strncpy(s, "00000000", 8);

}

void traffic_light::finish() {

    // Sent the quit signal to the PyRTL stopLight
    write(outFifo, "q", 1);
    close(inFifo);
    close(outFifo);

}

// Send a command to the PyRTL stopLight every clock
bool traffic_light::tick(SST::Cycle_t) {

    write(outFifo, s, 8);
    // Clear the command so we don't send the same command over and over
    // need to receive a command from the port
    strncpy(s, "00000000", 8);

    // Read from the PyRTL stopLight to see what state the light is in
    char r[2] = "\0";
    read(inFifo, r, 1);
    std::string c;
    switch (r[0]) {
        case '0':
            c = "green";
            break;
        case '1':
            c = "yellow";
            break;
        case '2':
            c = "red";
            break;
    }

    port->send(new SST::Interfaces::StringEvent(c));
    return false;

}
