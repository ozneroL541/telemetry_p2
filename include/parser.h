#ifndef PARSER_H
#define PARSER_H

#include <cstdint>
#include <regex>
#include <cstring>
#include <string>

#ifndef MAX_CAN_MESSAGE_SIZE
#define MAX_CAN_MESSAGE_SIZE 20
#endif
#define MAX_LOG_SIZE (MAX_CAN_MESSAGE_SIZE + 12)

#define SPECIAL_ID 0x0A0
#define START_PAYLOADS {0x6601, 0xFF01}
#define STOP_PAYLOAD 0x66FF

class parsed_msg {
private:
    /** Message timestamp */
    time_t timestamp;
    /** Message ID */
    uint16_t id;
    /** Message payload */
    uint64_t payload;
    /** 
     * Parses the message to extract ID and payload
     * @param msg The message content
     * @param length The message length
     */
    void parse_message(const char * msg, const int length);
    /** 
     * Get the message content
     * @return The message content
     */
    char * get_msg();
public:
    /** 
     * Constructor.
     * @param timestamp The message timestamp
     * @param msg The message content
     * @param length The message length
     */
    parsed_msg(const time_t timestamp, const char * msg, const int length);
    /** 
     * Void Constructor
     * TMCH
     */
    parsed_msg();
    /** 
     * Destructor
     */
    ~parsed_msg();
    /** 
     * Get the message ID
     * @return The message ID
     */
    uint16_t get_id();
    /** 
     * Checks if the message is empty
     * @return 1 if the message is empty, 0 otherwise
     */
    char is_empty();
    /** 
     * Returns whether the message has a special start/stop id
     * @return 1 if the message is special, 0 otherwise
     */
    char is_special_id();
    /** 
     * Checks if the message is a start payload
     * @return 1 if the message is a start payload, 0 otherwise
     */
    char is_start_payload();
    /** 
     * Checks if the message is a stop payload
     * @return 1 if the message is a stop payload, 0 otherwise
     */
    char is_stop_payload();
    /** 
     * Checks if the message is a start message
     * @return 1 if the message is a start message, 0 otherwise
     */
    char is_start_message();
    /** 
     * Checks if the message is a stop message
     * @return 1 if the message is a stop message, 0 otherwise
     */
    char is_stop_message();
    /**
     * Get a log with unix timestamp and message
     * @return The message log
     */
    char * get_log();
    /** 
     * Get the message timestamp
     * @return The message timestamp
     */
    time_t get_timestamp();
};

#endif
