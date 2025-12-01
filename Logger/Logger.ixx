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
#ifndef NDEBUG
        if (!condition) throw std::runtime_error(trace());
#endif
    }

    /**
     * If NDEBUG is not set - logs to file on false condition (with stacktrace)
     * @param condition
     * @param source log filename
     */
    export void assert_log(const bool condition, const std::string &source) {
#ifndef NDEBUG
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
#endif
    }

    export void assert_abort(const bool condition) {
#ifndef NDEBUG
        assert(condition);
#endif
    }

    /**
     * Writes message to log file
     * @param msg message to write
     * @param source log filename
     */
    export void writeLog(const char* msg, const std::string &source) {
#ifndef NDEBUG
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
#endif
    }
}


