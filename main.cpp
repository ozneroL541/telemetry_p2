#include <stdio.h>
#include <iostream>
#include "include/can_reader.h"
#include "include/fsm.h"
#include "include/can_reader.h"

extern "C"{
    #include "fake_receiver.h"
}

int main(void){
    /** Finite State Machine */
    finite_state_machine * fsm = new finite_state_machine();
    /** Thread to receive CAN data */
    pthread_t receive_thread;
    /** Thread to run the finite state machine */
    pthread_t machine_thread;

    open_can("candump.log");
    pthread_create(&receive_thread, NULL, read_can_data_thread, (void*)fsm);
    pthread_create(&machine_thread, NULL, start_machine, (void*)fsm);
    pthread_join(receive_thread, NULL);
    close_can();
    pthread_join(machine_thread, NULL);
    delete fsm;
    return 0;
}
