///

/*
https://linux.die.net/man/7/inotify for
 details on why and hows of this implementation */

/* random guid included in header def, to prevent conflicts */
#ifndef FILESYSNOTIFY_FSNOTIFY_H_A0B65073_7154_4D2A_8B92_0482649C1D41
#define FILESYSNOTIFY_FSNOTIFY_H_A0B65073_7154_4D2A_8B92_0482649C1D41

#define FSN_BASETYPE_FILE
#define FSN_BASETYPE_DIR

#define FSN_EVENT_ACCESS        IN_ACCESS
#define FSN_EVENT_MODIFYATTRIB  IN_ATTRIB
#define FSN_EVENT_WRITE         IN_CLOSE_WRITE
#define FSN_EVENT_NOWRITE       IN_CLOSE_NOWRITE
#define FSN_EVENT_CREATE        IN_CREATE
#define FSN_EVENT_DELETE        IN_DELETE
#define FSN_EVENT_DELETE_SELF   IN_DELETE_SELF
#define FSN_EVENT_MODIFY        IN_MODIFY
#define FSN_EVENT_MOVE_SELF     IN_MOVE_SELF
#define FSN_EVENT_MOVED_FROM    IN_MOVED_FROM
#define FSN_EVENT_MOVED_TO      IN_MOVED_TO
#define FSN_EVENT_OPEN          IN_OPEN

#define FSN_EVENT               IN_ALL_EVENTS

#define HIGHRESTIME                 std::chrono::time_point<std::chrono::high_resolution_clock>

#include <cstdint>
#include <cstdio>
#include <linux/limits.h>
#include <sys/inotify.h>
#include <unistd.h>
#include "fsnotify.h"
#include <sys/inotify.h>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <poll.h>
#include <malloc.h>
#include <unistd.h>
#include <vector>
#include <functional>
#include <algorithm>
#include "idir.h"


///
namespace FSNotify {
    ///
    typedef enum FSNRESULT {
        FSN_OK              = 0,
        FSN_GENERIC_ERR,
        FSN_DIRLIST_EMPTY,
        FSN_INOTIFYINIT_ERR,
        FSN_ALLOCATION_ERR,
        FSN_WATCH_ERR
    } FSNRESULT;
    ///
    typedef struct {
        char                        buffer;         /* event buffer */
        int32_t                     fd;             /* file descriptor */
        std::vector<int>        watchd;         /* watch descriptors */
        uint32_t                    watchd_n;       /* number of watch descriptors */
        struct pollfd               polld[2];       /* two poll descriptors */
        nfds_t                      polld_n;        /* integral type number of polls */
        int32_t                     poll_id;        /* poll id for event id purposes */
    } FSNContainer_t;
    ///
    typedef struct {
        uint32_t                    eventmask;
        uint32_t                    cookie;
        uint8_t                     filepath[PATH_MAX];
        uint8_t                     is_file;
        std::string                 message;
        HIGHRESTIME                 timestamp;
    }FSNEventLog_t;

    typedef struct {
        uint32_t                    eventmask;
        uint32_t                    cookie;
        uint8_t                     filepath[PATH_MAX];
        uint8_t                     is_file;
        uint8_t                     message[256];
        uint64_t                    timestamp;
    }FSNEventLogSerializable_t;

    class FSNotifyHandler {
        ///
    private:
        FSNContainer_t fsnc;
        std::vector<std::string> dir_index;
        std::vector<FSNEventLog_t> write_bufferspace;
        int pipefd[2];


    public:
        FSNotifyHandler();
        ~FSNotifyHandler();

        FSNRESULT init( std::vector<std::string> directory_index, int pipefd[2]);

        FSNRESULT fdvector_to_watch(std::vector<std::string> dirindex, FSNContainer_t * fsncptr);

        FSNEventLog_t eventlog_construct(const struct inotify_event *event,
                                         HIGHRESTIME timestamp, std::string filepath, bool isfile);

        void eventlog_print(FSNotify::FSNEventLog_t *eventlog);

        FSNEventLogSerializable_t eventlog_to_serializable(FSNotify::FSNEventLog_t *eventlog);

        FSNRESULT start();

        template <typename ...Args> FSNRESULT start( std::function<void(Args...)>&& function, Args... args );

    private:

        void handle_events( int32_t fileDescriptor, std::vector<int> watchDescriptor,
                            int32_t numWatched, std::vector<std::string> watchedDirectories );

        static void _default_callback(void);
    };
}
#endif //FILESYSNOTIFY_FSNOTIFY_H_A0B65073_7154_4D2A_8B92_0482649C1D41
