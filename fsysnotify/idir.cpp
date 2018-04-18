/**
 *
 */

/**
 * INCLUDES
 */
#include "idir.h"

/**
 *
 */
namespace FSNotify {
    /**
     * FUNCTION DEFINITIONS
     */

    /**
     *
     */
    void idirrec(std::string pdir, std::vector<std::string> &directory_list) {
        DIR *dir;
        struct dirent *ent;
        std::vector<std::string>::iterator iterator;

        dir = opendir(pdir.c_str());
        if (dir != nullptr) {
            while ((ent = readdir(dir)) != nullptr) {
                if (ent->d_type == DT_DIR) {
#ifndef HIDDEN_DIR
                    if ((ent->d_name)[0] != HIDDEN_DIR_SYMBOL) {
#endif
                        std::string path;
                        path.append(pdir).append("/").append(ent->d_name);
                        //std::cout << path << std::endl;
                        iterator = directory_list.end();
                        directory_list.insert(iterator, path);
                        idirrec(path, directory_list);
#ifndef HIDDEN_DIR
                    }
#endif
                }
            }
            /* add the base folder */
            iterator = directory_list.end();
            directory_list.insert(iterator, pdir);
        }
        closedir(dir);
    }
    /**
     *
     */
}
/**
 *
 */

