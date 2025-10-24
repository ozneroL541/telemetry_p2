#ifndef FSM_H
#define FSM_H

#include "msg.h"
#include "parser.h"
#include "statistics_handler.h"

#include <stdio.h>
#include <pthread.h>
#include <list>
#include <string>
#include <semaphore.h>

/** 
 * State of the machine
 */
typedef enum {
    /** Idle state */
    IDLE,
    /** Running state */
    RUNNING
} state_t;

/** 
 * Finite State Machine class
 * @class finite_state_machine
 */
class finite_state_machine {
    private:
        /** Current state of the machine */
        state_t state;
        /** Mutex for state access */
        pthread_mutex_t  state_mx;
        /** List of received datas */
        std::list<message> data_list;
        /** Mutex for data access */
        pthread_mutex_t  data_mx;
        /** Semaphore for data availability */
        sem_t data_sem;
        /** List of parsed messages */
        std::list<parsed_msg> parsed_list;
        /** Mutex for parsed_list access */
        pthread_mutex_t parsed_list_mx;
        /** Semaphore for parsed_list availability */
        sem_t parsed_list_sem;
        /** Log file pointer */
        FILE *log_file;
        /** Flag indicating if transmission is finished */
        char transmission_over;
        /** Mutex for transmission_over access */
        pthread_mutex_t transmission_over_mx;
        /** Statistics handler */
        statistics_handler * stats_handler;
        /** Mutex for statistics_handler access */
        pthread_mutex_t stats_handler_mx;

        /** 
         * Transitions the state machine to the RUNNING state 
         */
        void transition_to_running();
        /** 
         * Transitions the state machine to the IDLE state
        */
        void transition_to_idle();
        /** 
         * Returns the first data in the data list
         * @return The first data in the list
         */
        message read_first_data();
        /** 
         * Returns the first parsed message in the parsed list
         * @return The first parsed message in the list
         */
        parsed_msg read_first_parsed_msg();
        /** 
         * Processes the parsed message in the IDLE state
         * @param pmsg The data to be processed
         */
        void idle_process(parsed_msg pmsg);
        /** 
         * Processes the parsed message in the RUNNING state
         * @param pmsg The data to be processed
         */
        void running_process(parsed_msg pmsg);
        /** 
         * Logs the message to the log file
         * @param pmsg The message to be logged
         */
        void log_message(parsed_msg pmsg);
        /** 
         * Updates statistics based on the parsed message
         * @param pmsg The message to update statistics with
         */
        void update_statistics(const parsed_msg pmsg);
        /** 
         * Do what is needed in RUNNING state
         * @param pmsg The data to be processed
         */
        void do_running_stuff(const parsed_msg pmsg);
        /** 
         * Returns whether the parsing is over
         * @return 1 if parsing is over, 0 otherwise
         */
        char is_parsing_over();
        /** 
         * Returns whether the processing is over
         * @return 1 if processing is over, 0 otherwise
         */
        char is_processing_over();
        /** 
         * Template function to add an element to a list
         * @param el The element to be added
         * @param list The list to which the element is added
         * @param mx The mutex for the list
         * @param sem The semaphore for the list
         */
        template<typename T>
        void add_el_to_list(const T el, std::list<T> &list, pthread_mutex_t &mx, sem_t &sem);
        /** 
         * Template function to read the first element from a list
         * @param list The list from which to read the element
         * @param mx The mutex for the list
         * @param sem The semaphore for the list
         * @return The first element from the list
         */
        template<typename T>
        T read_first(std::list<T> &list, pthread_mutex_t &mx, sem_t &sem);
    public:
        /** 
         * Constructor.
         * Initializes the state of the machine to IDLE.
         */
        finite_state_machine();
        /** 
         * Destructor.
         */
        ~finite_state_machine();
        /** 
         * Checks if the current state is IDLE 
         * @return 1 if the state is IDLE, 0 otherwise
         */
        char is_idle();
        /** 
         * Checks if the current state is RUNNING 
         * @return 1 if the state is RUNNING, 0 otherwise
         */
        char is_running();
        /** 
         * Sets the transmission over flag
         * @param status The status to set
         */
        void set_transmission_over(const char status);
        /** 
         * Gets the transmission over flag
         * @return The transmission over status
         */
        char is_transmission_over();
        /** 
         * Adds data to the data list
         * @param data The data to be added
         */
        void add_data(const message data);
        /** 
         * Adds a parsed message to the parsed list
         * @param pdata The parsed message to be added
         */
        void add_parsed_msg(const parsed_msg pdata);
        /** 
         * Parses data from the data list
         */
        void parse_data();
        /** 
         * Thread function to parse data
         * from the data list
         */
        void parse_data_t();
        /** 
         * Processes the data in the data list
         */
        void process_data();
        /** 
         * Thread function to process data
         * from the data list
         */
        void process_data_t();
        /** 
         * Executes the elaboration activities
         * the finite state machine is supposed to do
         */
        void run();
};

/**
 * Thread function to parse data
 * from the data list
 * @param arg The finite state machine pointer
 * @return arg
 */
void * parse_data_thread(void * arg );
/**
 * Thread function to process data
 * from the data list
 * @param arg The finite state machine pointer
 * @return arg
 */
void * process_data_thread(void * arg );
/**
 * Thread function to start the finite state machine
 * @param arg The finite state machine pointer
 * @return arg
 */
void * start_machine(void * arg );

#endif
