/**
 *
 */

#ifndef FILESYSNOTIFY_IDIR_H
#define FILESYSNOTIFY_IDIR_H

/**
 * INCLUDES
 */
#include <iostream>
#include <vector>
#include <dirent.h>
#include <pwd.h>
#include <unistd.h>
/**
 *
 */

/**
 * USER CONFIG
 */
// 0 - indexer will ignore hidden directories
// setting this to 1 will cause significant load on the system
#define INDEX_HIDDEN_DIRECTORIES            0
// default POSIX is 0x2E
#define HIDDEN_DIR_SYMBOL                   0x2E
// default home path is /home/$USER
#define HOME_PATH                           (getpwuid(getuid())->pw_dir)

/**
 * SYSTEM CONFIG DO-NOT-MODIFY
 */
#if INDEX_HIDDEN_DIRECTORIES == 1
#define HIDDEN_DIR
#endif
/**
 *
 */
namespace FSNotify {
    /**
    * FUNCTION DECLARATIONS
    */

    /**
    * @brief Function recursively indexes the directory paths, starting from a parent directory.
    * @param pdir parent directory path
    * @param directory_list directory list (passed by address)
    */
    void idirrec(std::string pdir, std::vector<std::string> &directory_list);
    /**
     *
     */
}
/**
 *
 */
#endif //FILESYSNOTIFY_IDIR_H
