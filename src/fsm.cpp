#include "fsm.h"
#include <stdio.h>

static char is_start_message(std::string message) {
    for (const auto& start_msg : start_messages) {
        if (start_msg == message) {
            return 1;
        }
    }
    return 0;
}

static char is_stop_message(std::string message) {
    return stop_message == message ? 1 : 0;
}

finite_state_machine::finite_state_machine() {
    pthread_mutex_init(&this->state_mx, NULL);
    pthread_mutex_init(&this->data_mx, NULL);
    sem_init(&this->data_sem, 0, 0);
    this->state = IDLE;
    data_list.clear();
}

finite_state_machine::~finite_state_machine() {
    pthread_mutex_destroy(&this->state_mx);
    pthread_mutex_destroy(&this->data_mx);
    sem_destroy(&this->data_sem);
    data_list.clear();
}

void finite_state_machine::transition_to_running() {
    pthread_mutex_lock(&this->state_mx);
    this->state = RUNNING;
    pthread_mutex_unlock(&this->state_mx);
    /* TODO: Implement any additional logic for transitioning to RUNNING state */
}

void finite_state_machine::transition_to_idle() {
    pthread_mutex_lock(&this->state_mx);
    this->state = IDLE;
    pthread_mutex_unlock(&this->state_mx);
    /* TODO: Implement any additional logic for transitioning to IDLE state */
}

char finite_state_machine::is_idle() {
    char result = 1;
    pthread_mutex_lock(&this->state_mx);
    result = this->state == IDLE ? 1 : 0;
    pthread_mutex_unlock(&this->state_mx);
    return result;
}

char finite_state_machine::is_running() {
    char result = 1;
    pthread_mutex_lock(&this->state_mx);
    result = this->state == RUNNING ? 1 : 0;
    pthread_mutex_unlock(&this->state_mx);
    return result;
}

void finite_state_machine::add_data(std::string data) {
    pthread_mutex_lock(&this->data_mx);
    this->data_list.push_back(data);
    pthread_mutex_unlock(&this->data_mx);
    sem_post(&this->data_sem);
}

std::string finite_state_machine::read_first_data() {
    std::string first_data;
    sem_wait(&this->data_sem);
    pthread_mutex_lock(&this->data_mx);
    if (!data_list.empty()) {
        first_data = data_list.front();
        data_list.pop_front();
    }
    pthread_mutex_unlock(&this->data_mx);
    return first_data;
}

void finite_state_machine::receive_data() {
    /** Receive data from the CAN bus */
    char message[MAX_CAN_MESSAGE_SIZE];
    /** Number of bytes received */
    int bytes_received = can_receive(message);

    /* Check for errors */
    if (bytes_received < 0) {
        fprintf(stderr, "Error receiving CAN message\n");
        return;
    }

    /* Convert received bytes into a string and add to the list */
    this->add_data(std::string(message, (bytes_received > 0) ? bytes_received : 0));
}

void finite_state_machine::idle_process(std::string data) {
    if (is_start_message(data.c_str())) {
        this->transition_to_running();
    }
}

void finite_state_machine::running_process(std::string data) {
    if (is_stop_message(data.c_str())) {
        this->transition_to_idle();
    } else {
        /* TODO: Implement data processing logic for RUNNING state */
    }
}

void finite_state_machine::process_data() {
    /** Data to process */
    std::string data = this->read_first_data();
    /* Check if data is empty */
    if (data.empty()) {
        return;
    }
    /* Process the data based on the current state */
    if (this->is_idle()) {
        this->idle_process(data);
    } else if (this->is_running()) {
        this->running_process(data);
    }
}
