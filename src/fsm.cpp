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
    this->log_file = NULL;
    this->state = IDLE;
    data_list.clear();
}

finite_state_machine::~finite_state_machine() {
    pthread_mutex_destroy(&this->state_mx);
    pthread_mutex_destroy(&this->data_mx);
    sem_destroy(&this->data_sem);
    data_list.clear();
    if (this->log_file != NULL) {
        fflush(this->log_file);
        fclose(this->log_file);
    }
}

void finite_state_machine::transition_to_running() {
    time_t current_timestamp = time(NULL);
    std::string filename = std::to_string(current_timestamp) + ".log";
    pthread_mutex_lock(&this->state_mx);
    this->state = RUNNING;
    this->log_file = fopen(filename.c_str(), "a");
    if (this->log_file == NULL) {
        fprintf(stderr, "Failed to open log file: %s\n", filename.c_str());
        this->state = IDLE;
    }
    pthread_mutex_unlock(&this->state_mx);
}

void finite_state_machine::transition_to_idle() {
    pthread_mutex_lock(&this->state_mx);
    this->state = IDLE;
    if (this->log_file != NULL) {
        fflush(this->log_file);
        fclose(this->log_file);
        this->log_file = NULL;
    }
    pthread_mutex_unlock(&this->state_mx);
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

void finite_state_machine::add_data(message data) {
    pthread_mutex_lock(&this->data_mx);
    this->data_list.push_back(data);
    pthread_mutex_unlock(&this->data_mx);
    sem_post(&this->data_sem);
}

message finite_state_machine::read_first_data() {
    /** First data message from the list */
    message data = message("", 0); /* TODO: Check memory leaks */
    sem_wait(&this->data_sem);
    pthread_mutex_lock(&this->data_mx);
    if (!data_list.empty()) {
        data = data_list.front();
        data_list.pop_front();
    } else {
        fprintf(stderr, "Data list is empty when trying to read first data\n");
    }
    pthread_mutex_unlock(&this->data_mx);
    return data;
}

void finite_state_machine::receive_data() {
    /** Receive data from the CAN bus */
    char m[MAX_CAN_MESSAGE_SIZE];
    /** Number of bytes received */
    int bytes_received = can_receive(m);
    /** Create a message object */
    message msg(m, bytes_received);

    /* Check for errors */
    if (bytes_received < 0) {
        fprintf(stderr, "Error receiving CAN message\n");
        return;
    }

    this->add_data(msg);
}

void finite_state_machine::idle_process(message data) {
    if (is_start_message(data.get_msg())) {
        this->transition_to_running();
        this->log_message(data);
    }
}

void finite_state_machine::log_message(message data) {
    if (this->log_file != NULL) {
        fprintf(
            this->log_file, 
            "%s\n", 
            data.get_log()
        );
    }
}

void finite_state_machine::running_process(message data) {
    this->log_message(data);
    if (is_stop_message(data.get_msg())) {
        this->transition_to_idle();
    }
}

void finite_state_machine::process_data() {
    /** Data to process */
    message data = this->read_first_data();
    /* Check if data is empty */
    if (data.get_msg().empty()) {
        return;
    }
    /* Process the data based on the current state */
    if (this->is_idle()) {
        this->idle_process(data);
    } else if (this->is_running()) {
        this->running_process(data);
    }
}
