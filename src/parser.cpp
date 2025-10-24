#include "parser.h"

void parsed_msg::parse_message(const char * msg, const int length) {
    /** Regular expression for parsing the message */
    const char regex[] = "^([0-9A-Fa-f]{3})#((?:[0-9A-Fa-f]{2}){1,8})$";
    /** Regular expression object */
    std::regex re(regex);
    /** String representation of the message */
    std::string msg_str(msg, length);
    /** Match results */
    std::smatch m;

    if (std::regex_match(msg_str, m, re) && m.size() >= 3) {
        this->id = static_cast<uint16_t>(std::stoul(m[1].str(), nullptr, 16));
        this->payload = static_cast<uint64_t>(std::stoull(m[2].str(), nullptr, 16));
    } else {
        fprintf(stderr, "Error: Invalid message format: %s\n", msg);
    }
}

parsed_msg::parsed_msg(const timespec timestamp, const char * msg,const int length) {
    this->timestamp = timestamp;
    this->id = 0;
    this->payload = 0;
    this->parse_message(msg, length);
}

parsed_msg::parsed_msg() {
    this->timestamp.tv_sec = 0;
    this->timestamp.tv_nsec = 0;
    this->id = 0;
    this->payload = 0;
}

parsed_msg::~parsed_msg() {}

uint16_t parsed_msg::get_id() {
    return this->id;
}

char parsed_msg::is_empty() {
    return this->timestamp.tv_sec == 0 && this->id == 0;
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
    /** Static buffer for the message */
    static char buffer[MAX_CAN_MESSAGE_SIZE];
    /** Number of bytes in payload */
    int payload_bytes = 0;
    /** Auxiliary variable for payload processing */
    uint64_t temp_payload = this->payload;
    
    if (temp_payload == 0) {
        payload_bytes = 1;
    } else {
        while (temp_payload > 0) {
            payload_bytes++;
            temp_payload >>= 8;
        }
    }
    if (payload_bytes % 2 != 0) {
        payload_bytes++;
    }

    sprintf(buffer, "%03X#", this->id);
    std::string result = buffer;
    
    for (int i = payload_bytes - 1; i >= 0; i--) {
        uint8_t byte = (this->payload >> (i * 8)) & 0xFF;
        sprintf(buffer, "%02X", byte);
        result += buffer;
    }
    snprintf(buffer, sizeof(buffer), "%s", result.c_str());
    
    return buffer;
}

char * parsed_msg::get_log() {
    /** Generate a log entry with timestamp and message */
    static char log_buffer[MAX_LOG_SIZE];
    /** Get the length of the message */
    int len = snprintf(
        log_buffer, 
        MAX_LOG_SIZE, 
        "%ld %s", 
        this->timestamp.tv_sec,
        this->get_msg()
    );
    
    if (len <= 0 || len >= MAX_LOG_SIZE) {
        log_buffer[0] = '\0';
    }
    return log_buffer;
}

timespec parsed_msg::get_timestamp() {
    return this->timestamp;
}
