#include "fsm.h"
#include "msg.h"
#include "msg_info.h"
#include <stdio.h>

int main(void) {
    finite_state_machine fsm;

    /* Simulate receiving a start message */
    message start_msg("0A0#6601", 8);
    fsm.add_data(start_msg);
    fsm.process_data();

    if (fsm.is_running()) {
        printf("FSM transitioned to RUNNING state successfully.\n");
    } else {
        printf("FSM failed to transition to RUNNING state.\n");
    }

    /* Simulate receiving a stop message */
    message stop_msg("0A0#66FF", 8);
    fsm.add_data(stop_msg);
    fsm.process_data();

    if (fsm.is_idle()) {
        printf("FSM transitioned to IDLE state successfully.\n");
    } else {
        printf("FSM failed to transition to IDLE state.\n");
    }

    return 0;
}
