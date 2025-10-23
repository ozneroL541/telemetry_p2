#ifndef CAN_READER_H
#define CAN_READER_H

#include "fsm.h"

#include <stdio.h>

extern "C"{
    #include "fake_receiver.h"
}

/** 
 * Reads data from the CAN bus and adds it to the FSM data list
 * @param fsm Pointer to the finite state machine
 * @return 0 on success, -1 on failure
 */
char read_can_data(finite_state_machine* fsm);
/** 
 * Thread function to read data from the CAN bus
 * @param arg Pointer to the finite state machine
 * @return arg
 */
void *read_can_data_thread(void *arg);


#endif
