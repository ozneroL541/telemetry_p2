#include <stdio.h>
#include <iostream>
#include "include/fsm.h"
#include "include/can_reader.h"

extern "C"{
    #include "fake_receiver.h"
}

/** 
 * Thread wrapper for processing data
 * @param arg TMCH
 * @return arg
 */
static void* process_thread_entry(void* arg) {
    return reinterpret_cast<finite_state_machine*>(arg)->process_data_thread(arg);
}

int main(void){
    finite_state_machine * fsm = new finite_state_machine();
    pthread_t receive_thread;
    pthread_t process_thread;
    open_can("candump.log");
    pthread_create(&receive_thread, NULL, read_can_data_thread, (void*)fsm);
    pthread_create(&process_thread, NULL, process_thread_entry, (void*)fsm);
    pthread_join(receive_thread, NULL);
    pthread_join(process_thread, NULL);
    close_can();
    delete fsm;
    return 0;
}
