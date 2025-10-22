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
