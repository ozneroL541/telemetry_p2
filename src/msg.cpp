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

std::string message::get_msg() {
    return std::string(msg, length);
}

char * message::get_log() {
    /** Generate a log entry with timestamp and message */
    static char log_buffer[MAX_LOG_SIZE];
    int len = snprintf(
        log_buffer, 
        MAX_LOG_SIZE, 
        "%ld %.*s", 
        this->timestamp, 
        this->length, 
        this->msg
    );
    printf("Generated log of length %d: %s\n", len, log_buffer);
    return log_buffer;
}
