#include "fsm.h"
#define DEBUG

finite_state_machine::finite_state_machine() {
    pthread_mutex_init(&this->state_mx, NULL);
    pthread_mutex_init(&this->data_mx, NULL);
    pthread_mutex_init(&this->transmission_over_mx, NULL);
    pthread_mutex_init(&this->parsed_list_mx, NULL);
    pthread_mutex_init(&this->stats_handler_mx, NULL);
    sem_init(&this->parsed_list_sem, 0, 0);
    sem_init(&this->data_sem, 0, 0);
    this->transmission_over = 0;
    this->log_file = NULL;
    this->state = IDLE;
    this->stats_handler = NULL;
    data_list.clear();
    parsed_list.clear();
}

finite_state_machine::~finite_state_machine() {
    this->transition_to_idle();
    pthread_mutex_destroy(&this->state_mx);
    pthread_mutex_destroy(&this->data_mx);
    pthread_mutex_destroy(&this->transmission_over_mx);
    pthread_mutex_destroy(&this->parsed_list_mx);
    pthread_mutex_destroy(&this->stats_handler_mx);
    data_list.clear();
    parsed_list.clear();
    sem_destroy(&this->data_sem);
    sem_destroy(&this->parsed_list_sem);
}

template <typename T>
void finite_state_machine::add_el_to_list(const T el, std::list<T> &list, pthread_mutex_t &mx, sem_t &sem) {
    pthread_mutex_lock(&mx);
    list.push_back(el);
    pthread_mutex_unlock(&mx);
    sem_post(&sem);
}

template <typename T>
T finite_state_machine::read_first(std::list<T> &list, pthread_mutex_t &mx, sem_t &sem) {
    /** Element to return */
    T el;
    sem_wait(&sem);
    pthread_mutex_lock(&mx);
    if (!list.empty()) {
        el = list.front();
        list.pop_front();
    } else {
        fprintf(stderr, "List is empty when trying to read first element\n");
    }
    pthread_mutex_unlock(&mx);
    return el;
}

void finite_state_machine::transition_to_running() {
    timespec current_timestamp;
    clock_gettime(CLOCK_REALTIME, &current_timestamp);
    const std::string filename = std::to_string(current_timestamp.tv_sec) + ".log";
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
    pthread_mutex_lock(&this->stats_handler_mx);
    pthread_mutex_lock(&this->state_mx);
    if (this->stats_handler != NULL) {
        this->stats_handler->print_stats();
        delete this->stats_handler;
        this->stats_handler = NULL;
    }
    pthread_mutex_unlock(&this->stats_handler_mx);
    this->state = IDLE;
    if (this->log_file != NULL) {
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

void finite_state_machine::set_transmission_over(const char status) {
    pthread_mutex_lock(&this->transmission_over_mx);
    this->transmission_over = status;
    pthread_mutex_unlock(&this->transmission_over_mx);
}

char finite_state_machine::is_transmission_over() {
    char result = 0;
    pthread_mutex_lock(&this->transmission_over_mx);
    result = this->transmission_over;
    pthread_mutex_unlock(&this->transmission_over_mx);
    return result;
}

void finite_state_machine::add_data(const message data) {
    add_el_to_list<message>(data, this->data_list, this->data_mx, this->data_sem);
}

message finite_state_machine::read_first_data() {
    return read_first<message>(this->data_list, this->data_mx, this->data_sem);
}

parsed_msg finite_state_machine::read_first_parsed_msg() {
    return read_first<parsed_msg>(this->parsed_list, this->parsed_list_mx, this->parsed_list_sem);
}

void finite_state_machine::add_parsed_msg(const parsed_msg pdata) {
    add_el_to_list<parsed_msg>(pdata, this->parsed_list, this->parsed_list_mx, this->parsed_list_sem);
}

void finite_state_machine::parse_data() {
    /** Data to parse */
    message data = this->read_first_data();
    if (!data.is_empty()) {
        this->add_parsed_msg(data.get_parsed_msg());
    }
}

void finite_state_machine::parse_data_t() {
    while (!this->is_transmission_over() || !this->is_parsing_over()) {
        this->parse_data();
    }
}

void finite_state_machine::log_message(parsed_msg pmsg) {
    if (this->log_file != NULL) {
        fprintf(
            this->log_file, 
            "%s\n", 
            pmsg.get_log()
        );
        fflush(this->log_file);
    }
}

void finite_state_machine::update_statistics(const parsed_msg pmsg) {
    pthread_mutex_lock(&this->stats_handler_mx);
    if (this->stats_handler == NULL) {
        this->stats_handler = new statistics_handler(pmsg);
    } else {
        this->stats_handler->add_message(pmsg);
    }
    pthread_mutex_unlock(&this->stats_handler_mx);   
}

void finite_state_machine::do_running_stuff(const parsed_msg pmsg) {
    this->log_message(pmsg);
    this->update_statistics(pmsg);
}

char finite_state_machine::is_processing_over(){
    char are_lists_empty = 0;
    pthread_mutex_lock(&this->data_mx);
    are_lists_empty += this->data_list.empty() ? 1 : 0;
    pthread_mutex_unlock(&this->data_mx);
    pthread_mutex_lock(&this->parsed_list_mx);
    are_lists_empty += this->parsed_list.empty() ? 1 : 0;
    pthread_mutex_unlock(&this->parsed_list_mx);
    #ifdef DEBUG
    printf("is_processing_over: %d\n", !are_lists_empty);
    #endif
    return are_lists_empty? 1 : 0;
}

char finite_state_machine::is_parsing_over(){
    char result = 1;
    pthread_mutex_lock(&this->data_mx);
    result = this->data_list.empty() ? 1 : 0;
    #ifdef DEBUG
    printf("is_parsing_over: %d\n", result);
    #endif
    pthread_mutex_unlock(&this->data_mx);
    return result;
}


void finite_state_machine::idle_process(parsed_msg pmsg) {
    if (pmsg.is_start_message()) {
        this->transition_to_running();
        this->do_running_stuff(pmsg);
    }
}

void finite_state_machine::running_process(parsed_msg pmsg) {
    this->do_running_stuff(pmsg);
    if (pmsg.is_stop_message()) {
        this->transition_to_idle();
    }
}

void finite_state_machine::process_data() {
    /** Data to process */
    parsed_msg pmsg = this->read_first_parsed_msg();
    /* Check if the message is valid */
    if (pmsg.is_empty()) {
        #ifdef DEBUG
        printf("Parsed message is empty\n");
        #endif
        return;
    }
    /* Process the data based on the current state */
    if (this->is_idle()) {
        #ifdef DEBUG
        printf("IDLE: %s\n", pmsg.get_log());
        #endif
        this->idle_process(pmsg);
    } else if (this->is_running()) {
        #ifdef DEBUG
        printf("RUNNING: %s\n", pmsg.get_log());
        #endif
        this->running_process(pmsg);
    }
}

void finite_state_machine::process_data_t() {
    while (!this->is_transmission_over() || !this->is_processing_over()) {
        this->process_data();
    }
}

void finite_state_machine::run() {
    /** Thread identifiers */
    pthread_t parse_thread;
    pthread_t process_thread;

    /** Create threads */
    pthread_create(&parse_thread, NULL, parse_data_thread, this);
    pthread_create(&process_thread, NULL, process_data_thread, this);

    /** Join threads */
    pthread_join(parse_thread, NULL);
    pthread_join(process_thread, NULL);
}

void *parse_data_thread(void *arg) {
    /** Finite State Machine */
    finite_state_machine *fsm = static_cast<finite_state_machine *>(arg);
    fsm->parse_data_t();
    pthread_exit(arg);
}

void *process_data_thread(void *arg) {
    /** Finite State Machine */
    finite_state_machine *fsm = static_cast<finite_state_machine *>(arg);
    fsm->process_data_t();
    pthread_exit(arg);
}

void * start_machine(void * arg) {
    /** Finite State Machine */
    finite_state_machine *fsm = static_cast<finite_state_machine *>(arg);
    fsm->run();
    pthread_exit(arg);
}
