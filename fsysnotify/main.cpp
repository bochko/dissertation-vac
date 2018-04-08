#include <iostream>
#include <vector>
#include <cstring>
#include "idir.h"
#include "fsnotify.h"

int main(int argc, char** argv) {
    /*if(argc != 3) {
        std::cout << "Unsuitable arguments passed to executable \"fsnotify\"" << std::endl;
        std::cout << "Usage: fsnotify <pipe read identifier> <pipe write identifier>" << std::endl << "terminating process..." << std::endl;
        exit(EXIT_FAILURE);
    }*/

    FSNotify::FSNotifyHandler handler;
    std::vector<std::string> idir;

    /* home path is a defined preprocessor symbol, therefore
     * totally unmodifiable, strcat canNOT change its contents */
    FSNotify::idirrec(HOME_PATH, idir);

    std::cout << "Handler initialization has started..." << std::endl;
    handler.init(idir, 0);
    std::cout << "Handler initialization has finished..." << std::endl;
    std::cout << "Number of directories watched: " << idir.size() << std::endl;
    handler.start();
    exit(EXIT_FAILURE);
}