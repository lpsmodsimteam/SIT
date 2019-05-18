int TRFFCLGHTFSM_NPORTS = 8;

enum traffic_light_fsm_ports {
    __pid__, trffclghtfsm_clock, trffclghtfsm_load, trffclghtfsm_start_green, trffclghtfsm_green_time, trffclghtfsm_yellow_time, trffclghtfsm_red_time, trffclghtfsm_state
};
