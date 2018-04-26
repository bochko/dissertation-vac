/**
 *
 */
/**
 * INCLUDES from header, for reference's sake
#include <cstdint>
#include <cstdio>
#include <linux/limits.h>
#include <sys/inotify.h>
#include <unistd.h>
#include "fsnotify.h"
#include <sys/inotify.h>
#include <cstdlib>
#include <iostream>
#include <poll.h>
#include <malloc.h>
#include <unistd.h>
#include <vector>
#include <functional>
#include <algorithm>
#include "idir.h"
 */
/**
 * INCLUDES
 */
#include <cstring>
#include "fsnotify.h"

/**
 *
 */

FSNotify::FSNotifyHandler::FSNotifyHandler() {
    /**
     * do nothing, with style
     */
    ;
}

FSNotify::FSNotifyHandler::~FSNotifyHandler() {
    /**
     * free(nothing, apparently)
     */
    ;
}

FSNotify::FSNRESULT
FSNotify::FSNotifyHandler::init(std::vector<std::string> directory_index,
                                int pipefd) {

    FSNRESULT res;
    /**
     * initialize the inode watcher
     */
    this->dir_index = directory_index;
    // population of needed number of watch descriptors
    if (directory_index.empty()) {
        res = FSN_DIRLIST_EMPTY;
    } else {
        this->fsnc.watchd_n = (uint32_t) directory_index.size(); //change to uint64_t?
    }

    // population of file descriptor
    this->fsnc.fd = inotify_init1(
            IN_NONBLOCK); // DO NOT use IN_NONBLOCK, this will cause inotify to NOT wait for events
    if (this->fsnc.fd == -1) {
        perror("[!] inotify_init has failed to latch on.");
        res = FSN_INOTIFYINIT_ERR;
    }

    // allocation of watch descriptors
    this->fsnc.watchd.reserve((size_t) this->fsnc.watchd_n);
    /*
    old routine
    this->fsnc.watchd = (int32_t *) calloc(this->fsnc.watchd_n, sizeof(int32_t));
    if (this->fsnc.watchd == nullptr) {
        perror("calloc() failed to allocate blocks");
        res = FSN_ALLOCATION_ERR;
    }
     */

    res = this->fdvector_to_watch(directory_index, &(this->fsnc));
    if (res != FSN_OK) {
        return res;
    }

    /* piece up designated pipe information */
    //memcpy(this->pipefd, pipefd, sizeof(int) * 2);
    this->pipe_write_end = pipefd;
    /* res may not entirely reflect all errors that occured in initialization
     * but it should show the last one, which can help debugging */
    return res;
}

FSNotify::FSNRESULT
FSNotify::FSNotifyHandler::start() {
    /**
     * run start with default callback
     */
    this->start(std::function<void(void)>(_default_callback));
}

/* the most disgusting function declaration in the universe
 * allows for passing an arbitrary callback function, with an arbitrary number of arguments.
 * Cannot be used with VOID or NULL, but it can be used with "std::function<void(void)> **func" */
template<typename ...Args>
FSNotify::FSNRESULT
FSNotify::FSNotifyHandler::start(std::function<void(Args...)> &&function,
                                 Args... args) {
    //Prepare for polling
    this->fsnc.polld_n = 2;
    //Console input (???)
    this->fsnc.polld[0].fd = STDIN_FILENO;
    this->fsnc.polld[0].events = POLLIN;
    //INotify input
    this->fsnc.polld[1].fd = fsnc.fd;
    this->fsnc.polld[1].events = POLLIN;

    uint64_t poll_count = 0;

    while (true) {
        /* check if buffer vector is not empty and send data to named pipe if it is */
        if (!this->write_bufferspace.empty()) {
            auto iterator = this->write_bufferspace.begin();
            FSNRESULT piperes = FSN_OK;

            std::for_each(this->write_bufferspace.begin(), this->write_bufferspace.end(), [this, &iterator](FSNEventLog_t eventnfo) {
                // serialize
                FSNEventLogSerializable_t serialized_data = eventlog_to_serializable(&eventnfo);
                // SEND IT FLYING THROUGH SPACE (the pipe)
                /**
                 * THIS WILL BLOCK AS PIPE SHOULD HAVE BEEN CREATED WITHOUT THE O_NONBLOCK flag
                 * ALSO, IF pipefd UNINITIALIZED IT WILL WRITE TO NULL, which is stdin
                 */
                if(write(this->pipe_write_end, &serialized_data, sizeof(FSNEventLogSerializable_t)) == -1) {
                    std::cout << " PIPE FD = " << this->pipe_write_end << " ERRNO = " << errno <<std::endl;
                }

                this->write_bufferspace.erase(this->write_bufferspace.begin());
                /* sending element from buffer after sending it to space, address should remain constant */
                //std::cout << "[!] sent and erasing buffer element... addr::" << (__ptr_t) &eventnfo << std::endl;
            });

        }
        /* polling number may overflow, even though it's a 64 bit number */
        this->fsnc.poll_id = poll(this->fsnc.polld, fsnc.polld_n, -1);
        if (this->fsnc.poll_id == -1) {
            if (errno == EINTR) {
                perror("FSNotify was interrupted while polling");
                exit(EXIT_FAILURE);
            }
        }
        if (this->fsnc.poll_id > 0) {
            if (this->fsnc.polld[0].revents & POLLIN) {
                /* Console input is available. Empty stdin and quit */
                while (read(STDIN_FILENO, &(this->fsnc.buffer), 1) > 0 && (this->fsnc.buffer) != '\n')
                    continue;
                break;
            }
            if (this->fsnc.polld[1].revents & POLLIN) {

                if (function != nullptr) {
                    function(args...);
                }
                /* Inotify events are available */
                handle_events(this->fsnc.fd, this->fsnc.watchd, this->fsnc.watchd_n, this->dir_index);
            }
        }
        poll_count += 1;
    }
    return FSN_OK;
}

void FSNotify::FSNotifyHandler::_default_callback() {
    /**
     * literally do nothing once
     */
    ;
}

void
FSNotify::FSNotifyHandler::handle_events(int32_t fileDescriptor,
                                         std::vector<int> watchDescriptor,
                                         int32_t numWatched,
                                         std::vector<std::string> watchedDirectories) {

    /* Some systems cannot read integer variables if they are not
              properly aligned. On other systems, incorrect alignment may
              decrease performance. Hence, the buffer used for reading from
              the inotify file descriptor should have the same alignment as
              struct inotify_event. */

    char buf[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    uint32_t i;
    ssize_t len;
    char *ptr;
    bool isfile;
    std::string filepath;
    FSNEventLog_t eventlog;

    /* Loop while events can be read from inotify file descriptor. */

    for (;;) {
        /* Read some events. */
        len = read(fileDescriptor, buf, sizeof buf);
        if (len == -1 && errno != EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        /* If the nonblocking read() found no events to read, then
           it returns -1 with errno set to EAGAIN. In that case,
           we exit the loop. */
        if (len <= 0)
            break;
        /* Loop over all events in the buffer */
        for (ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
            event = (const struct inotify_event *) ptr;
            /* find the watch descriptor matching the event to get DIR */
            for (i = 1; i < numWatched; ++i) {
                if (watchDescriptor[i] == event->wd) {
                    /* stamp the event handling */
                    HIGHRESTIME event_timestamp = std::chrono::high_resolution_clock::now();
                    /* if there is a filename then put it in the buffer */
                    if (event->len) {
                        filepath = watchedDirectories.at(i) + "/" + event->name;
                    } else {
                        filepath = watchedDirectories.at(i);
                    }
                    /* check type of event */
                    if (event->mask & IN_ISDIR) {
                        isfile = false;
                        if (event->mask & IN_CREATE) {
                            /* add directory to the watch */
                            this->fsnc.watchd.insert(this->fsnc.watchd.end(),
                                                     inotify_add_watch(this->fsnc.fd, filepath.c_str(),
                                                                       FSN_EVENT));
                            this->dir_index.insert(this->dir_index.end(), filepath);
                            this->fsnc.watchd_n++;

                            std::cout << "A new directory has been added to the watchlist: " << filepath << std::endl;
                        }
                    } else {
                        isfile = true;
                    }
                    eventlog = eventlog_construct(event, event_timestamp, filepath, isfile);
                    this->write_bufferspace.insert(this->write_bufferspace.end(), eventlog);
                    eventlog_print(&eventlog);
                    break;
                }
            }
        }
    }
}

FSNotify::FSNRESULT
FSNotify::FSNotifyHandler::fdvector_to_watch(std::vector<std::string> dirindex,
                                             FSNotify::FSNContainer_t *fsnc) {
    uint32_t i = 0;
    std::for_each(dirindex.begin(), dirindex.end(),
                  [&fsnc, &i](std::string dir_path) {
                      /* in order to monitor all events the bit mask should be set
                       * to IN_ALL_EVENTS / aliased FSN_EVENT, add the returned watch descriptor
                       * to the common watch descriptor vector */
                      fsnc->watchd.insert(fsnc->watchd.end(), inotify_add_watch(fsnc->fd, dir_path.c_str(),
                                                                                FSN_EVENT));
                      //std::cout << dir_path << " ADDED TO WATCH" << std::endl;
                      if (fsnc->watchd[i] == -1) {
                          fprintf(stderr, "Cannot watch '%s'\n", dir_path.c_str());
                          perror("inotify_add_watch");
                          return FSNotify::FSN_WATCH_ERR;

                      }
                      i += 1;
                  }
    );
    return FSNotify::FSN_OK;
}

FSNotify::FSNEventLog_t
FSNotify::FSNotifyHandler::eventlog_construct(const struct inotify_event *event,
                                              HIGHRESTIME timestamp,
                                              std::string filepath,
                                              bool isfile) {

    FSNEventLog_t logobj;
    /* add timestamp */
    logobj.timestamp = timestamp;
    /* add event cookie */
    logobj.cookie = event->cookie;
    logobj.eventmask = event->mask;
    strncpy((char *) logobj.filepath, filepath.c_str(),
            FILENAME_MAX); // truncates to filename max, which is funnily enough not the largest filename you can have
    isfile ? logobj.is_file = 1 : logobj.is_file = 0;

    if (event->mask & IN_ACCESS)
        logobj.message = "File/Directory access.";
    else if (event->mask & IN_ATTRIB)
        logobj.message = "File/Directory metadata modified.";
    else if (event->mask & IN_CLOSE_WRITE)
        logobj.message = "File closed after being written to.";
    else if (event->mask & IN_CLOSE_NOWRITE)
        logobj.message = "File closed without being written to.";
    else if (event->mask & IN_CREATE)
        logobj.message = "File/Directory created. ";
    else if (event->mask & IN_DELETE)
        logobj.message = "File/Directory deleted";
    else if (event->mask & IN_DELETE_SELF)
        logobj.message = "File/Directory deleted self (special event)";
    else if (event->mask & IN_MODIFY)
        logobj.message = "File/Directory modified (special event)";
    else if (event->mask & IN_MOVE_SELF)
        logobj.message = "File/Directory moved self (special event)";
    else if (event->mask & IN_MOVED_FROM)
        logobj.message = "File/Directory moved from source.";
    else if (event->mask & IN_MOVED_TO)
        logobj.message = "File/Directory moved to target.";
    else if (event->mask & IN_OPEN)
        logobj.message = "File/Directory opened.";
    else
        logobj.message = "Unrecognized event. This shouldn't happen"
                " unless inotify event identifiers have been reimplemented.";
    return logobj;
}

void FSNotify::FSNotifyHandler::eventlog_print(FSNotify::FSNEventLog_t *eventlog) {
    std::cout << "time: " << eventlog->timestamp.time_since_epoch().count() /* << std::endl*/ << " | "
              << eventlog->message /*<< std::endl*/ << " | "
              << "path: " << eventlog->filepath /*<< std::endl*/ << " | type: " << (bool) eventlog->is_file << " | "
              << "cookie: " << eventlog->cookie << std::endl;
    return;
}

FSNotify::FSNEventLogSerializable_t FSNotify::FSNotifyHandler::eventlog_to_serializable(FSNotify::FSNEventLog_t *eventlog) {
    FSNEventLogSerializable_t eventlog_pod;
    eventlog_pod.is_file = eventlog->is_file;
    eventlog_pod.cookie = eventlog->cookie;
    eventlog_pod.eventmask = eventlog->eventmask;
    strncpy((char*)eventlog_pod.message, eventlog->message.c_str(), 256); eventlog_pod.message[256] = 0;
    strncpy((char*)eventlog_pod.filepath, (const char*) eventlog->filepath, FILENAME_MAX); eventlog_pod.filepath[FILENAME_MAX] = 0;
    eventlog_pod.timestamp = static_cast<uint64_t>(eventlog->timestamp.time_since_epoch().count());
    return eventlog_pod;
}




