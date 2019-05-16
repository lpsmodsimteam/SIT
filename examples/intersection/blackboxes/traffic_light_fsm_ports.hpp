int TRAFFIC_LIGHT_FSM_NPORTS = 8;

enum traffic_light_fsm_ports {
    __pid__, _clock, load, start_green, green_time, yellow_time, red_time, state
};
