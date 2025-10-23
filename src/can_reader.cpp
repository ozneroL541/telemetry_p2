#include "can_reader.h"

char read_can_data(finite_state_machine *fsm) {
    /** Receive data from the CAN bus */
    char m[MAX_CAN_MESSAGE_SIZE] = {0};
    /** Number of bytes received */
    int bytes_received = can_receive(m);
    /** Create a message object */
    message msg(m, bytes_received);

    /* Check for errors */
    if (bytes_received > 0) {
        fsm->add_data(msg);
        return 0;
    } else {
        fprintf(stderr, "Error receiving CAN message\n");
        return -1;
    } 
}

void *read_can_data_thread(void *arg) {
    /** Finite state machine pointer */
    finite_state_machine *fsm = static_cast<finite_state_machine *>(arg);
    /** Exit flag */
    char exit = 0;

    while (!fsm->is_transmission_over() && !exit) {
        exit = read_can_data(fsm);
    }
    fsm->set_transmission_over(1);
    pthread_exit(arg);
}
