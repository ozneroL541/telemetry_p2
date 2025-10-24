#ifndef include_MSG_H
#define include_MSG_H

#include "parser.h"

#include <ctime>
#include <cstdint>
#include <cstdio>
#ifndef MAX_CAN_MESSAGE_SIZE
#define MAX_CAN_MESSAGE_SIZE 20
#endif

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
    timespec timestamp;
public:
    /**
     * Constructor.
     * @param msg The message content
     * @param length The message length
     */
    message(const char msg[MAX_CAN_MESSAGE_SIZE], const int length);
    /**
     * Void constructor
     * TMCH
     */
    message();
    /** Destructor */
    ~message();
    /** 
     * Checks if the message is empty
     * @return 1 if the message is empty, 0 otherwise
     */
    char is_empty();
    /**
     * Get the timestamp of the message
     * @return The message timestamp
     */
    timespec get_timestamp();
    /**
     * Get the parsed message
     * @return The parsed message
     */
    parsed_msg get_parsed_msg();
};

#endif
