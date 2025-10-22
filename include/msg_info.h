#ifndef MSG_INFO_H
#define MSG_INFO_H

#include <cstdint>
#include <string>

class message_info
{
private:
    /** Message ID */
    uint16_t id;
    /** Message payload */
    char * payload;
public:
    message_info(std::string message);
    ~message_info();
};

message_info::message_info(std::string message)
{
}

message_info::~message_info()
{
}


#endif
