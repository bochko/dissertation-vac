//
// Created by boyan on 18/04/18.
//

#include <cstdlib>
#include <unistd.h>
#include "fsinformer.h"
#include "../fsysnotify/fsnotify.h"
#include <cstring>

volatile sig_atomic_t FSInformer::FSInformerHandler::flag_release_lock;

std::ofstream FSInformer::FSInformerHandler::dbfd;

FSInformer::FSInformerHandler::FSInformerHandler() {
    /* ensure we read from stdin by default */
    this->pipe_read_end = STDIN_FILENO;
    this->database_path = std::string(".defaultdb");
}

FSInformer::FSInformerHandler::~FSInformerHandler() = default;

void FSInformer::FSInformerHandler::init(int pipe_read_end, std::string database_path) {
    signal(SIGUSR1, this->unlock_database_signal_handler);
    signal(SIGINT, this->release_lock);
    signal(SIGTERM, this->release_lock);
    signal(SIGKILL, this->release_lock);
    signal(SIGABRT, this->release_lock);
    this->pipe_read_end = pipe_read_end;
    this->database_path = database_path;
    std::cout << "FSysInformer initialized: pipe = " << this->pipe_read_end << " database path = "
              << this->database_path << std::endl;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void FSInformer::FSInformerHandler::start(void) {

    for (;;) {
        bool changed = false;
        auto *log = (FSNotify::FSNEventLogSerializable_t *) malloc(sizeof(FSNotify::FSNEventLogSerializable_t));

        std::cout << "FSysInformer :: Waiting to read from pipe";
        read(this->pipe_read_end, log, sizeof(FSNotify::FSNEventLogSerializable_t));
        std::cout << "FSysInformer :: Read whole package from pipe:" << std::endl;
        std::cout << "OBJ:  " << log->filepath << std::endl;
        std::cout << "MASK: " << log->eventmask << std::endl;
        if(log->is_file) {
            std::cout << "IS FILE? " << "yes" << std::endl;
        } else {
            std::cout << "IS FILE? " << "no" << std::endl;
        }


            std::cout << "CAPTURED EVENT IS FILE..." << std::endl;
            /* handle move events */
            if (log->cookie != 0) {
                if (log->eventmask & IN_MOVED_FROM || log->eventmask & IN_MOVED_TO) {
                    changed = true;
                    std::cout << "PREPARING TO RECORD A MOVE..." << std::endl;
                    auto iterator = unresolved_events.begin();
                    auto found = false;
                    /* this is very unoptimized as it traverses everything */
                    std::for_each(this->unresolved_events.begin(), this->unresolved_events.end(),
                                  [&](FSNotify::FSNEventLogSerializable_t event) {
                                      if (log->cookie == event.cookie && log->eventmask & FSN_EVENT_MOVED_FROM &&
                                          event.eventmask & FSN_EVENT_MOVED_TO) {
                                          /* file moved TO path is the one */
                                          strncpy((char *) &(this->database.last_moved_file),
                                                  (const char *) event.filepath, FILENAME_MAX);
                                          found = true;
                                      } else if (log->cookie == event.cookie &&
                                                 log->eventmask & FSN_EVENT_MOVED_TO &&
                                                 event.eventmask & FSN_EVENT_MOVED_FROM) {
                                          strncpy((char *) &(this->database.last_moved_file),
                                                  (const char *) log->filepath, FILENAME_MAX);
                                          strncpy((char *) &(this->database.last_moved_origin), (const char *) event.filepath, FILENAME_MAX);
                                          std::cout << "LAST MOVED FILE: " << this->database.last_moved_file
                                                    << std::endl;
                                          if(strcmp((const char *)this->database.last_moved_origin, (const char *)this->database.last_edited_file) == 0) {
                                              memcpy(this->database.last_edited_file, this->database.last_moved_file, FILENAME_MAX);
                                          } else if(strcmp((const char *)this->database.last_moved_origin, (const char *)this->database.last_created_file) == 0) {
                                              memcpy(this->database.last_created_file, this->database.last_moved_file, FILENAME_MAX);
                                          }
                                          found = true;
                                      }
                                      iterator += 1;
                                  });
                    if (!found) {
                        this->unresolved_events.insert(this->unresolved_events.begin(), *log);
                    }
                }
            }

                /* handle file creation */
            else if (log->eventmask & IN_CREATE) {
                changed = true;
                strncpy((char *) &(this->database.last_created_file), (const char *) log->filepath, FILENAME_MAX);
                std::cout << "LAST CREATED FILE: " << this->database.last_created_file << std::endl;
            }

                /* handle file editing */
            else if (log->eventmask & IN_CLOSE_WRITE) {
                changed = true;
                strncpy((char *) &(this->database.last_edited_file), (const char *) log->filepath, FILENAME_MAX);
                std::cout << "LAST EDITED FILE: " << this->database.last_edited_file << std::endl;
            }

        if (FSInformer::FSInformerHandler::flag_release_lock == 0 && changed) {
            /* write out the changes,
             * this step is needed only before the release of the lock
             * as in any other state no other process can
             * read it */
            FSInformer::FSInformerHandler::dbfd.open("/home/boyan/Dissertation/dissertation-main/.hidden/data.db",
                                                     std::ofstream::out | std::ios::binary | std::ofstream::trunc);

            size_t dblen = strlen((const char *) database.last_edited_file) +
                           strlen((const char *) database.last_created_file) +
                           strlen((const char *) database.last_moved_file);
            std::cout << "WRITING " << dblen << " SYMBOLS IN FILE..." << std::endl;

            std:: cout << "LEF " << database.last_edited_file << std::endl << "LCF " << database.last_created_file << std::endl
                       << "LMF " << database.last_moved_file << std::endl;

            dbfd << "LEF " << database.last_edited_file << std::endl << "LCF " << database.last_created_file << std::endl
                 << "LMF " << database.last_moved_file << std::endl;

            dbfd.close();

        }
        /* file edited code */
        free(log);
    }
}

#pragma clang diagnostic pop

void FSInformer::FSInformerHandler::unlock_database_signal_handler(int arg) {
    std::cout << "FSysInformer :: SIGNAL SIGUSR1 TRIGGERED [!!!]" << std::endl;
    FSInformer::FSInformerHandler::flag_release_lock = 1;
}

void FSInformer::FSInformerHandler::release_lock(int arg) {
    dbfd.close();
}