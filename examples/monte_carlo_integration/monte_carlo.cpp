#include "monte_carlo.hpp"

monte_carlo::monte_carlo(SST::ComponentId_t id, SST::Params& params)
    : SST::Component(id),

      // collect all the parameters from the project driver
      seed(params.find<uint16_t>("SEED", 0)),
      output(params.find<bool>("OUTPUT", true)),

      x_rand32_din_link(configureLink("x_rand32_din")),
      x_rand32_dout_link(configureLink(
          "x_rand32_dout",
          new SST::Event::Handler<monte_carlo>(this, &monte_carlo::x_rand32))),

      y_rand32_din_link(configureLink("y_rand32_din")),
      y_rand32_dout_link(configureLink(
          "y_rand32_dout",
          new SST::Event::Handler<monte_carlo>(this, &monte_carlo::y_rand32))),

      div_x_din_link(configureLink("div_x_din")),
      div_x_dout_link(configureLink(
          "div_x_dout",
          new SST::Event::Handler<monte_carlo>(this, &monte_carlo::div_x))),

      div_y_din_link(configureLink("div_y_din")),
      div_y_dout_link(configureLink(
          "div_y_dout",
          new SST::Event::Handler<monte_carlo>(this, &monte_carlo::div_y))),

      sum_sq_din_link(configureLink("sum_sq_din")),
      sum_sq_dout_link(configureLink(
          "sum_sq_dout",
          new SST::Event::Handler<monte_carlo>(this, &monte_carlo::sum_sq))) {

    m_output.init("\033[93mmonte_carlo-" + getName() + "\033[0m -> ", 1, 0,
                  SST::Output::STDOUT);

    m_output.setVerboseLevel(output);
    // Just register a plain clock for this simple example
    registerClock(
        "1Hz", new SST::Clock::Handler<monte_carlo>(this, &monte_carlo::tick));

    registerAsPrimaryComponent();
    primaryComponentDoNotEndSim();
}

void monte_carlo::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");
}

void monte_carlo::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());
    // std::fclose(m_fp);
}

bool monte_carlo::tick(SST::Cycle_t) {}

void monte_carlo::x_rand32(SST::Event* ev) {
    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (se && m_keep_recv) {

        x = std::stof(se->getString());
    }

    delete se;
}

void monte_carlo::y_rand32(SST::Event* ev) {
    auto* se = dynamic_cast<SST::Interfaces::StringEvent*>(ev);
    if (se && m_keep_recv) {

        y = std::stof(se->getString());
    }

    delete se;
}

void monte_carlo::div_x(SST::Event* ev) {}

void monte_carlo::div_y(SST::Event* ev) {}

void monte_carlo::sum_sq(SST::Event* ev) {}
