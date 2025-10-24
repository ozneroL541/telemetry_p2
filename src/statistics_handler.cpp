#include "statistics_handler.h"

id_stat::id_stat(time_t timestamp) {
    this->number_of_messages = 1;
    this->sum_of_intervals = 0.0;
    this->last_timestamp = timestamp;
}

id_stat::~id_stat() {}

void id_stat::update_intervals(time_t timestamp) {
    this->sum_of_intervals += difftime(timestamp, this->last_timestamp);;
}

double id_stat::get_mean_time() {
    return this->sum_of_intervals / this->number_of_messages;
}

void id_stat::update_stats(time_t timestamp) {
    this->update_intervals(timestamp);
    this->last_timestamp = timestamp;
    this->number_of_messages++;
}

char * id_stat::get_csv_line() {
    /** Static buffer for CSV line */
    static char csv_line[64];
    /** Format the CSV line */
    int len = snprintf(
        csv_line,
        sizeof(csv_line),
        "%lu,%.3f",
        (unsigned long)this->number_of_messages,
        this->get_mean_time()
    );
    if (len <= 0 || len >= sizeof(csv_line)) {
        csv_line[0] = '\0';
    }
    return csv_line;
}

statistics_handler::statistics_handler(parsed_msg msg) {
    /** First statistic */
    id_stat initial_stat(msg.get_timestamp());

    this->timestamp = msg.get_timestamp();
    stats.clear();
    this->stats.insert({msg.get_id(), initial_stat});
}

statistics_handler::~statistics_handler() {
    stats.clear();
}

void statistics_handler::add_message(parsed_msg msg) {
    /** Find if the ID already exists */
    auto it = this->stats.find(msg.get_id());
    if (it != this->stats.end()) {
        /** Update existing statistics */
        it->second.update_stats(msg.get_timestamp());
    } else {
        /** Create new statistics for this ID */
        id_stat new_stat(msg.get_timestamp());
        this->stats.insert({msg.get_id(), new_stat});
    }
}

void statistics_handler::print_stats() {
    /** Filename based on the initial timestamp */
    std::string filename = std::to_string(this->timestamp) + ".csv";
    /** Open the file for writing */
    FILE *file = fopen(filename.c_str(), "a");
    if (file == NULL) {
        fprintf(
            stderr, 
            "Failed to open statistics file: %s\n", 
            filename.c_str()
        );
        return;
    }

    /** Write CSV header */
    fprintf(file, "ID,number_of_messages,mean_time\n");

    /** Write statistics for each message ID */
    for (auto& stat : this->stats) {
        fprintf(file, "%03X,%s\n", static_cast<unsigned int>(stat.first), stat.second.get_csv_line());
    }

    /** Close the file */
    fclose(file);
}
