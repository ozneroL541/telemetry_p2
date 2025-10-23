#ifndef STATISTICS_HANDLER_H
#define STATISTICS_HANDLER_H

#include "msg.h"

#include <cstdint>
#include <ctime>
#include <map>

/** 
 * Class to store statistics for a single message ID.
 * It keeps track of the number of messages received
 * and the mean time between messages.
 * @param id The message ID to track.
 * @class id_stat
 */
class id_stat {
private:
    /** The number of messages received */
    unsigned long number_of_messages;
    /** The mean time between messages */
    double mean_time;
    /** The timestamp of the last received message */
    time_t last_timestamp;

    /** 
     * Update the mean time between messages.
     * @param timestamp The timestamp of the new message.
    */
    void update_mean_time(time_t timestamp);
public:
    /** 
     * Constructor for id_stat class.
     * Initializes the message ID and sets
     * the number of messages and mean time to zero.
     * @param timestamp The timestamp of the first message.
    */
    id_stat(time_t timestamp);
    /** 
     * Destructor for id_stat class.
    */
    ~id_stat();
    /** 
     * Update statistics with a new timestamp.
     * @param timestamp The timestamp of the new message.
    */
    void update_stats(time_t timestamp);
    /** 
     * Get a CSV line representing the statistics.
     * @return A string in CSV format with ID, number of messages, and mean time.
    */
    char * get_csv_line();
};

/** 
 * Class to handle statistics for multiple message IDs.
 * It maintains a list of id_stat objects.
 * @class statistics_handler
 */
class statistics_handler {
private:
    /** The timestamp of the first received message */
    time_t timestamp;
    /** Ordered map of statistics for each message ID */
    std::map<uint16_t, id_stat> stats;
public:
    /** 
     * Constructor for statistics_handler class.
     * Initializes statistics with the first received message.
     * @param msg The first received message.
     */
    statistics_handler(message msg);
    /** 
     * Destructor for statistics_handler class.
     */
    ~statistics_handler();
    /** 
     * Add a new message to the statistics.
     * Updates existing statistics or creates new ones as needed.
     * @param msg The new message to add.
     */
    void add_message(message msg);
    /** 
     * Print the collected statistics to 
     * a CSV file named as the initial timestamp.
     */
    void print_stats();
};

#endif
