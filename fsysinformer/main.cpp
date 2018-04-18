#include <iostream>
#include "fsinformer.h"

int main(int argc, char **argv) {
    if(argc != 3) {
        std::cout << "Argument count mismatch" << std::endl;
        std::cout << "./fsinformer <pipe read fdesc> <database fpath>" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        FSInformer::FSInformerHandler handler = FSInformer::FSInformerHandler();
        //handler.init(atoi(argv[1]), std::string(argv[2]));
        handler.init(atoi(argv[1]), std::string(argv[2]));
        handler.start();
        exit(EXIT_SUCCESS);
    }
}