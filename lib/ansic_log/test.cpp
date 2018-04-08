#include <iostream>
#include "ansic_log.h"

int main(int argc, char ** argv) {
    ansic_log(LOG_CRITICAL, "Critical error here!");
    ansic_log(LOG_ERROR, "Error here!");
    ansic_log(LOG_WARN, "A slight mishap here!");
    ansic_log(LOG_INFO, "Nothing out of the ordinary...");
    ansic_log(LOG_DEBUG, "printf() debugging is bad and you should feel bad");
}