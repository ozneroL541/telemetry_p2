#ifndef FSM_H
#define FSM_H

#include "msg.h"

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

/** Start messages */
const static std::list<std::string> start_messages = {"0A0#6601", "0A0#FF01"};
/** Stop message */
const static std::string stop_message = "0A0#66FF";
/** 
 * Checks if the received message is a start message
 * @param message The received message
 * @return 1 if the message is a start message, 0 otherwise
 */
static char is_start_message(std::string message);
/** 
 * Checks if the received message is a stop message
 * @param message The received message
 * @return 1 if the message is a stop message, 0 otherwise
 */
static char is_stop_message(std::string message);

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
        /** Log file pointer */
        FILE *log_file;
        /** Flag indicating if transmission is finished */
        char transmission_over;
        /** Mutex for transmission_over access */
        pthread_mutex_t transmission_over_mx;

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
         * Processes data in the IDLE state
         * @param data The data to be processed
         */
        void idle_process(message data);
        /** 
         * Processes data in the RUNNING state
         * @param data The data to be processed
         */
        void running_process(message data);
        /** 
         * Logs the message to the log file
         * @param data The data to be logged
         */
        void log_message(message data);
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
        void set_transmission_over(char status);
        /** 
         * Gets the transmission over flag
         * @return The transmission over status
         */
        char is_transmission_over();
        /** 
         * Adds data to the data list
         * @param data The data to be added
         */
        void add_data(message data);
        /** 
         * Processes the data in the data list
         */
        void process_data();
        /** 
         * Thread function to process data
         * from the data list
         * @param arg TMCH
         * @return arg
         */
        void * process_data_thread(void * arg);
};

#endif
