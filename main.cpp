#include <stdio.h>
#include <iostream>
#include "include/fsm.h"

extern "C"{
    #include "fake_receiver.h"
}

/** 
 * Thread wrapper for receiving data
 * @param arg TMCH
 * @return arg
 */
static void* receive_thread_entry(void* arg) {
    return reinterpret_cast<finite_state_machine*>(arg)->receive_data_thread(arg);
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
    finite_state_machine fsm = finite_state_machine();
    pthread_t receive_thread;
    pthread_t process_thread;
    open_can("candump.log");
    pthread_create(&receive_thread, NULL, receive_thread_entry, (void*)&fsm);
    pthread_create(&process_thread, NULL, process_thread_entry, (void*)&fsm);
    pthread_join(receive_thread, NULL);
    pthread_join(process_thread, NULL);
    close_can();
    return 0;
}
