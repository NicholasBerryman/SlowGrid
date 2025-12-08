//
// Created by nickberryman on 15/11/25.
//
module;
#include <fstream>
#include <iostream>
#include <chrono>
#include <cassert>
#include <filesystem>
#include <stacktrace>
#include "Logger.h"

#define F_LOG "Log.txt"
//#define LOG_TO_STD 1

export module Logger;

// NOTE: EMPTY FUNCTION CALLS DON'T ALWAYS GET REMOVED EVEN IN O3?? -> Use macros in Logger.h that call these functions with ifndef NDEBUG


namespace Logging {
    bool init = false;
    std::string trace() {
        auto trace = std::stacktrace::current();
        return std::to_string(trace);
    }

    /**
     * If NDEBUG is not set - throws error on false condition (with stacktrace)
     * @param condition
     */
    export void assert_except(const bool condition) {
        if (!condition) throw std::runtime_error(trace());
    }

    /**
     * If NDEBUG is not set - logs to file on false condition (with stacktrace)
     * @param condition
     * @param source log filename
     */
    export void assert_log(const bool condition, const std::string &source) {
        if (!init) {
            init = true;
            std::ofstream(F_LOG);
        }
        if (!condition) {
#ifndef LOG_TO_STD
            std::ofstream LogFile(F_LOG, std::ios::app);
            LogFile <<
#else
            std::cout <<
#endif
                "[" << std::chrono::system_clock::now() << "] " <<
                "ERROR: " <<
                source <<
                std::endl;
#ifndef LOG_TO_STD
            LogFile.close();
#endif
        }
    }

    export void assert_abort(const bool condition) {
        assert(condition);
    }

    /**
     * Writes message to log file
     * @param msg message to write
     * @param source log filename
     */
    export void writeLog(const char* msg, const std::string &source) {
        if (!init) {
            init = true;
            std::ofstream(F_LOG);
        }
#ifndef LOG_TO_STD
        std::ofstream LogFile(F_LOG, std::ios::app);
        LogFile <<
#else
        std::cout <<
#endif
            "[" << std::chrono::system_clock::now() << "] " <<
            "(" << source << ") " <<
            msg << std::endl;
#ifndef LOG_TO_STD
        LogFile.flush();
#endif
    }
}


