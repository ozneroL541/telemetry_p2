#include "parser.h"

void parsed_msg::parse_message(const char * msg, const int length) {
    /** Regular expression for parsing the message */
    const char regex[] = "^(\b[0-9A-Fa-f]{3}\b)#(\b(?:[0-9A-Fa-f]{2}){1,8}\b)$";
    /** Regular expression object */
    std::regex re(regex);
    /** String representation of the message */
    std::string msg_str(msg, length);
    /** Match results */
    std::smatch m;

    if (std::regex_match(msg_str, m, re) && m.size() >= 3) {
        this->id = static_cast<uint16_t>(std::stoul(m[1].str(), nullptr, 16));
        this->payload = static_cast<uint64_t>(std::stoull(m[2].str(), nullptr, 16));
    }
}

parsed_msg::parsed_msg(const time_t timestamp, const char * msg,const int length) {
    this->timestamp = timestamp;
    this->id = 0;
    this->payload = 0;
    this->parse_message(msg, length);
}

parsed_msg::parsed_msg() {
    this->timestamp = 0;
    this->id = 0;
    this->payload = 0;
}

parsed_msg::~parsed_msg() {}

uint16_t parsed_msg::get_id() {
    return this->id;
}

char parsed_msg::is_empty() {
    return this->timestamp == 0 && this->id == 0;
}

char parsed_msg::is_special_id() {
    return this->id == SPECIAL_ID;
}

char parsed_msg::is_start_payload() {
    for (const auto &start_payload : START_PAYLOADS) {
        if (this->payload == start_payload) {
            return 1;
        }
    }
    return 0;
}

char parsed_msg::is_stop_payload() {
    return this->payload == STOP_PAYLOAD;
}

char parsed_msg::is_start_message() {
    return this->is_special_id() && this->is_start_payload();
}

char parsed_msg::is_stop_message() {
    return this->is_special_id() && this->is_stop_payload();
}

char * parsed_msg::get_msg() {
    /** Get the message content */
    static char msg_buffer[MAX_CAN_MESSAGE_SIZE];
    int len = snprintf(
        msg_buffer, 
        MAX_CAN_MESSAGE_SIZE, 
        "%03X#%llX",
        this->id, 
        this->payload
    );
    return msg_buffer;
}

char * parsed_msg::get_log() {
    /** Generate a log entry with timestamp and message */
    static char log_buffer[MAX_LOG_SIZE];
    /** Get the length of the message */
    int len = snprintf(
        log_buffer, 
        MAX_LOG_SIZE, 
        "%ld %s", 
        this->timestamp,
        this->get_msg()
    );
    
    if (len <= 0 || len >= MAX_LOG_SIZE) {
        log_buffer[0] = '\0';
    }
    return log_buffer;
}
