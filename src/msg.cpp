#include "msg.h"

message::message(const char msg[MAX_CAN_MESSAGE_SIZE], const int length) {
    char i = 0;
    this->timestamp = time(nullptr);
    this->length = length;
    for (i = 0; i < length && i < MAX_CAN_MESSAGE_SIZE; i++) {
        this->msg[i] = msg[i];
    }
}

message::~message() {}

time_t message::get_timestamp() {
    return this->timestamp;
}

std::string message::get_msg() {
    return std::string(msg, length);
}

char * message::get_log() {
    /** Generate a log entry with timestamp and message */
    static char log_buffer[MAX_LOG_SIZE];
    /** Get the length of the message */
    int len = snprintf(
        log_buffer, 
        MAX_LOG_SIZE, 
        "%ld %.*s", 
        this->timestamp, 
        this->length, 
        this->msg
    );
    
    if (len <= 0 || len >= MAX_LOG_SIZE) {
        log_buffer[0] = '\0';
    }
    return log_buffer;
}
