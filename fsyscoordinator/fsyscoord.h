//
// Created by boyan on 14/03/18.
//

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <vector>

#ifndef FSYSCOORD_FSYSCOORD_H
#define FSYSCOORD_FSYSCOORD_H

namespace FSCoord {
    typedef struct {
        bool running;
        pid_t pid;
        bool piped;
        int pipe_d[2];
    } FSProcessHandle_t;

    class FSCoordinator {
    private:
        int childnum;
        std::vector<FSProcessHandle_t> proc_history;

    };
}

#endif //FSYSCOORD_FSYSCOORD_H
