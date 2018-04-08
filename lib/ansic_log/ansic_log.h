//
// Created by boyan on 06/04/18.
//

#ifndef ANSIC_LOG_ANSIC_LOG_H
#define ANSIC_LOG_ANSIC_LOG_H

#include <cstdlib>

#define ANSI_ESC_CRITICAL               "\u001b[1m\u001b[41m\u001b[30m"
#define ANSI_ESC_ERROR                  "\u001b[1m\u001b[31m"
#define ANSI_ESC_WARN                   "\u001b[1m\u001b[33m"
#define ANSI_ESC_INFO                   "\u001b[1m"
#define ANSI_ESC_DEBUG                  "\u001b[36;1m"

#define ANSI_ESC_TIMESTAMP              "\u001b[1m"

#define ANSI_ESC_RESET                  "\u001b[0m"

#define LOG_CRITICAL                    0x01
#define LOG_ERROR                       0x02
#define LOG_WARN                        0x03
#define LOG_INFO                        0x04
#define LOG_DEBUG                       0x05

#define LOG_HEAD_CRITICAL               "CRITICAL"
#define LOG_HEAD_ERROR                  "ERROR"
#define LOG_HEAD_WARN                   "WARNING"
#define LOG_HEAD_INFO                   "INFO"
#define LOG_HEAD_DEBUG                  "DEBUG"

typedef struct {
    const char * head_esc_start;
    const char * head_message;
    const char * head_esc_end;
    const char * time_esc_start;
    char time_message[256];
    const char * time_esc_end;
    const char * append_message;
} log_seq_t;

void ansic_log(int level, const char * message);

int build_message(log_seq_t *log, int log_level, const char *append);

int print_message(log_seq_t *log);

int getchar_timestamp(char * buffer, size_t size_limit);

#endif //ANSIC_LOG_ANSIC_LOG_H
