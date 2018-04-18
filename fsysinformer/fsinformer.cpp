//
// Created by boyan on 18/04/18.
//

#include <cstdlib>
#include <unistd.h>
#include "fsinformer.h"
#include "../fsysnotify/fsnotify.h"
#include <fstream>
#include <sys/file.h>
#include <cstring>

volatile sig_atomic_t FSInformer::FSInformerHandler::flag_release_lock;

int FSInformer::FSInformerHandler::dbfd;

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
    std::cout << "FSysInformer initialized: pipe = " << this->pipe_read_end << " database path = " << this->database_path << std::endl;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void FSInformer::FSInformerHandler::start(void) {

    FSInformer::FSInformerHandler::dbfd = open(this->database_path.c_str(), O_WRONLY | O_CREAT);

    if(dbfd < 0) {
        std::cout << "Could not open database file for writing. Check locks" << std::endl;
        exit(EXIT_FAILURE);
    }

    for (;;) {
        if (dbfd > 0) {
            auto *log = (FSNotify::FSNEventLogSerializable_t *) malloc(sizeof(FSNotify::FSNEventLogSerializable_t));

            std::cout << "FSysInformer :: Waiting to read from pipe";
            read(this->pipe_read_end, log, sizeof(FSNotify::FSNEventLogSerializable_t));
            std::cout << "FSysInformer :: Read whole package from pipe:" <<std::endl;
            std::cout << "OBJ:  " << log->filepath <<std::endl;



            if (log->is_file) {
                /* handle move events */
                if (log->cookie != 0) {
                    if (log->eventmask & FSN_EVENT_MOVED_FROM || log->eventmask & FSN_EVENT_MOVED_TO) {
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
                                              found = true;
                                          }
                                          iterator += 1;
                                      });
                        if (found) {
                            found = false;
                            this->unresolved_events.erase(iterator);
                        } else {
                            this->unresolved_events.insert(this->unresolved_events.begin(), *log);
                        }
                    }
                }

                    /* handle file creation */
                else if (log->eventmask & FSN_EVENT_CREATE) {
                    strncpy((char *) &(this->database.last_created_file), (const char *) log->filepath, FILENAME_MAX);
                }

                    /* handle file editing */
                else if (log->eventmask & FSN_EVENT_WRITE) {
                    strncpy((char *) &(this->database.last_edited_file), (const char *) log->filepath, FILENAME_MAX);
                }

            } else {
                /* directory events to be monitored by nautilus extension */
            }
            free(log);
        }
        if (FSInformer::FSInformerHandler::flag_release_lock == 0) {
            /* write out the changes,
             * this step is needed only before the release of the lock
             * as in any other state no other process can
             * read it */

            ftruncate(dbfd, (__off_t) sizeof(FSNotify::FSNEventLogSerializable_t));

            size_t dblen = strlen((const char *) database.last_edited_file) +
                           strlen((const char *) database.last_created_file) +
                           strlen((const char *) database.last_moved_file);
            auto *wbuf = (char *) malloc(dblen);
            sprintf(wbuf, "%s\r\n%s\r\n%s\r\n", (const char *) database.last_edited_file,
                    (const char *) database.last_created_file, (const char *) database.last_moved_file);

            write(dbfd, wbuf, dblen);

            free(wbuf);
        } else {
            FSInformer::FSInformerHandler::flag_release_lock = 0;
            sleep(3);
        }
        /* file edited code */
    }
}

#pragma clang diagnostic pop

void FSInformer::FSInformerHandler::unlock_database_signal_handler(int arg) {
    std::cout << "FSysInformer :: SIGNAL SIGUSR1 TRIGGERED [!!!]" << std::endl;
    FSInformer::FSInformerHandler::flag_release_lock = 1;
}

void FSInformer::FSInformerHandler::release_lock(int arg) {
    close(FSInformer::FSInformerHandler::dbfd);
}