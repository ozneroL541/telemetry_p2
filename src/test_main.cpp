#include <stdio.h>
#include "parser.h"

int main() {
    // Create a parsed_msg object
    parsed_msg pmsg(1625079600, "004#E000", 8);

    printf("Message ID: %u\n", pmsg.get_id());
    printf("Is Empty: %d\n", pmsg.is_empty());
    printf("Is Start Message: %d\n", pmsg.is_start_message());
    printf("Is Stop Message: %d\n", pmsg.is_stop_message());
    printf("Log: %s\n", pmsg.get_log());

    return 0;
}