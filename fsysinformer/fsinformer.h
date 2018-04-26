//
// Created by boyan on 17/04/18.
//

#ifndef FSYSINFORMER_FSINFORMER_H
#define FSYSINFORMER_FSINFORMER_H

#define HIDDEN_PATH_DELIM       "/."

#include <cstdio>
#include <csignal>
#include <unistd.h>
#include <cstdint>
#include <string>
#include <deque>
#include <fstream>
#include "../fsysnotify/fsnotify.h"

namespace FSInformer {

    typedef struct {
        uint8_t last_edited_file[FILENAME_MAX];
        uint8_t last_created_file[FILENAME_MAX];
        uint8_t last_moved_file[FILENAME_MAX];
        uint8_t last_moved_origin[FILENAME_MAX];
    } FSInformerDatabase_t;

    class FSInformerHandler {

    private:
        int pipe_read_end;

        /* database filepath */
        std::string database_path;

        /* deque for storing unresolved events */
        std::vector<FSNotify::FSNEventLogSerializable_t> unresolved_events;

        /* local instanced database */
        FSInformerDatabase_t database;

    public:
        /* this will be modified by a signal handler AT ANY POINT
        * of execution, so it NEEDS
        * to be atomic and volatile */
        static volatile sig_atomic_t flag_release_lock;

        static std::ofstream dbfd;

        FSInformerHandler();
        ~FSInformerHandler();

        void init(int pipe_read_end, std::string database_path);
        void start(void);
        static void unlock_database_signal_handler(int arg);
        static void release_lock(int arg);
    };
}

#endif //FSYSINFORMER_FSINFORMER_H
