#ifndef STATISTICS_HANDLER_H
#define STATISTICS_HANDLER_H

#include <cstdint>
#include <ctime>

/** 
 * Class to store statistics for a single message ID.
 * It keeps track of the number of messages received
 * and the mean time between messages.
 * @param id The message ID to track.
 * @class id_stat
 */
class id_stat {
private:
    /** The message ID being tracked */
    uint16_t id;
    /** The number of messages received */
    unsigned long number_of_messages;
    /** The mean time between messages */
    double mean_time;
public:
    /** 
     * Constructor for id_stat class.
     * Initializes the message ID and sets
     * the number of messages and mean time to zero.
     * @param id The message ID to track.
    */
    id_stat(uint16_t id);
    /** 
     * Destructor for id_stat class.
    */
    ~id_stat();
    /** 
     * Update statistics with a new timestamp.
     * @param timestamp The timestamp of the new message.
    */
    void update_stats(time_t timestamp);
};
#endif
