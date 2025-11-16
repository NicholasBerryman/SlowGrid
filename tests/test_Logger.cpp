
//
// Created by nickberryman on 15/11/25.
//
#include <stdexcept>
#include "Logger.h"
import Logger;
void test(){
    // Test Passes
    Logging::assert_abort(1==0+1);
    Logging::assert_except(1==0+1);
    Logging::assert_log(1==0+1, LOGGER_TRACE);

    // Test Fails
    Logging::assert_log(1==1+1, LOGGER_TRACE); //Needs a manual check
    Logging::assert_log(1==1+1, LOGGER_TRACE); //Needs a manual check
    LOGGER_ASSERT_ERROR(Logging::assert_except(1==1+1);)

    //Test Messages
    Logging::writeLog("testtest", LOGGER_TRACE); //Needs a manual check

    //Test stacktraces (manual)
    //Logging::assert_except(1==2);

}

int main(int, char**) {
    test();
    return 0;
}

