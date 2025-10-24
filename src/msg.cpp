#include "msg.h"

message::message(const char msg[MAX_CAN_MESSAGE_SIZE], const int length) {
    char i = 0;
    this->timestamp = time(nullptr);
    this->length = length;
    for (i = 0; i < length && i < MAX_CAN_MESSAGE_SIZE; i++) {
        this->msg[i] = msg[i];
    }
}

message::message() {
    this->timestamp = 0;
    this->msg[0] = '\0';
    this->length = 0;
}

message::~message() {}

char message::is_empty() {
    return this->timestamp == 0 && this->length == 0;
}

time_t message::get_timestamp() {
    return this->timestamp;
}

parsed_msg message::get_parsed_msg() {
    parsed_msg pmsg(this->timestamp, this->msg, this->length);
    return pmsg;
}
