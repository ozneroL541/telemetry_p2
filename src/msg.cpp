#include "msg.h"

message::message(const char msg[MAX_CAN_MESSAGE_SIZE], const int length) {
    /** Index */
    char i = 0;
    
    clock_gettime(CLOCK_REALTIME, &this->timestamp);
    this->length = length;
    for (i = 0; i < length && i < MAX_CAN_MESSAGE_SIZE; i++) {
        this->msg[i] = msg[i];
    }
}

message::message() {
    this->timestamp.tv_sec = 0;
    this->timestamp.tv_nsec = 0;
    this->msg[0] = '\0';
    this->length = 0;
}

message::~message() {}

char message::is_empty() {
    return this->timestamp.tv_sec == 0 && this->length == 0;
}

timespec message::get_timestamp() {
    return this->timestamp;
}

parsed_msg message::get_parsed_msg() {
    parsed_msg pmsg(this->timestamp, this->msg, this->length);
    return pmsg;
}
