#include <stdio.h>
#include <iostream>
#include "include/can_reader.h"
#include "include/fsm.h"
#include "include/can_reader.h"

extern "C"{
    #include "fake_receiver.h"
}

void * can_handling_thread(void * arg){
    finite_state_machine * fsm = static_cast<finite_state_machine *>(arg);
    pthread_t receive_thread;
    open_can("candump.log");
    pthread_create(&receive_thread, NULL, read_can_data_thread, (void*)fsm);
    close_can();
    return arg;
}

int main(void){
    finite_state_machine * fsm = new finite_state_machine();
    pthread_t receive_thread;
    pthread_t process_thread;
    open_can("candump.log");
    pthread_create(&receive_thread, NULL, read_can_data_thread, (void*)fsm);
    fsm->start_machine();
    pthread_join(receive_thread, NULL);
    delete fsm;
    return 0;
}
