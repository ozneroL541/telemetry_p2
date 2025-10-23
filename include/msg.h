#ifndef include_MSG_H
#define include_MSG_H

#include <ctime>
#include <string>
#ifndef MAX_CAN_MESSAGE_SIZE
#define MAX_CAN_MESSAGE_SIZE 20
#endif

#define MAX_LOG_SIZE (MAX_CAN_MESSAGE_SIZE + 12)

/** 
 * Class to handle the messages containing 
 * their content, length and timestamp.
 * @class message
 */
class message {
private:
    /** Message content */
    char msg[MAX_CAN_MESSAGE_SIZE];
    /** Message length */
    int length;
    /** Timestamp of the message */
    time_t timestamp;
public:
    /**
     * Constructor.
     * @param msg The message content
     * @param length The message length
     */
    message(const char msg[MAX_CAN_MESSAGE_SIZE], const int length);
    /** Destructor */
    ~message();
    /**
     * Get the timestamp of the message
     * @return The message timestamp
     */
    time_t get_timestamp();
    /**
     * Get the message ID
     * @return The message ID
     */
    int16_t get_id();
    /**
     * Get the message content as a string
     * @return The message content
     */
    std::string get_msg();
    /**
     * Get a log with unix timestamp and message
     * @return The message log
     */
    char * get_log();
};

#endif
