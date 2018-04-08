//
// Created by boyan on 06/04/18.
//

#include "ansic_log.h"
#include <iostream>
#include <cstring>

void ansic_log(int level, const char * message) {
    log_seq_t * log = (log_seq_t *) malloc (sizeof(log_seq_t));
    if(build_message(log, level, message)) {
        print_message(log);
    }
}

int build_message(log_seq_t *log, int log_level, const char *append) {
    switch (log_level) {
        case LOG_CRITICAL:
            log->head_esc_start = ANSI_ESC_CRITICAL;
            log->head_message = LOG_HEAD_CRITICAL;
            log->head_esc_end = ANSI_ESC_RESET;
            break;
        case LOG_ERROR:
            log->head_esc_start = ANSI_ESC_ERROR;
            log->head_message = LOG_HEAD_ERROR;
            log->head_esc_end = ANSI_ESC_RESET;
            break;
        case LOG_WARN:
            log->head_esc_start = ANSI_ESC_WARN;
            log->head_message = LOG_HEAD_WARN;
            log->head_esc_end = ANSI_ESC_RESET;
            break;
        case LOG_INFO:
            log->head_esc_start = ANSI_ESC_INFO;
            log->head_message = LOG_HEAD_INFO;
            log->head_esc_end = ANSI_ESC_RESET;
            break;
        case LOG_DEBUG:
            log->head_esc_start = ANSI_ESC_DEBUG;
            log->head_message = LOG_HEAD_DEBUG;
            log->head_esc_end = ANSI_ESC_RESET;
            break;
        default:
            return 0;
    }
    log->time_esc_start = ANSI_ESC_TIMESTAMP;
    char * buffer = (char *) calloc(256, sizeof(char));
    getchar_timestamp(buffer, 256 * sizeof(char));
    strncpy(log->time_message, buffer, 256);
    log->time_esc_end = ANSI_ESC_RESET;
    log->append_message = append;
    free(buffer);
    return 1;
}

int print_message(log_seq_t *log) {
    if (log->head_esc_start != nullptr && log->head_message != nullptr && log->head_esc_end != nullptr &&
        log->time_esc_start != nullptr && log->time_message != nullptr && log->time_esc_end != nullptr &&
        log->append_message != nullptr) {
        printf("%s[%s]%s%s[%s]%s :: %s\n",
               log->head_esc_start,
               log->head_message,
               log->head_esc_end,
               log->time_esc_start,
               log->time_message,
               log->time_esc_end,
               log->append_message);
        return 1;
    } else {
        return 0;
    }
}

int getchar_timestamp(char *buffer, size_t size_limit) {
    const char *format = "%d:%d:%d";
    char temp[512];
    time_t t;
    struct tm *tm;
    int req_len;
    t = time(nullptr);
    tm = localtime(&t);
    req_len = snprintf(temp, 512, format,
                       tm->tm_hour,
                       tm->tm_min,
                       tm->tm_sec) + 1;
    if(size_limit < req_len) {
        return 0;
    }
    snprintf(buffer, size_limit, format,
             tm->tm_hour,
             tm->tm_min,
             tm->tm_sec);
    return 1;
}