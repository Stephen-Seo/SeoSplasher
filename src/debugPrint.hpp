#ifndef SEOSPLASHER_DEBUG_PRINT_HPP
#define SEOSPLASHER_DEBUG_PRINT_HPP

#include <iostream>

enum class DPLevel {
    DP_ERROR,
    DP_WARNING,
    DP_INFO,
    DP_VERBOSE,
    DP_DEBUG
};

#ifndef NDEBUG
#define SS_DPRINT(type, ...) \
    do { \
        std::clog << __FILE__ << ':' << __LINE__ << ' '; \
        switch(type) { \
        case DPLevel::DP_ERROR: \
            std::clog << "ERROR "; \
            break; \
        case DPLevel::DP_WARNING: \
            std::clog << "WARNING "; \
            break; \
        case DPLevel::DP_INFO: \
            std::clog << "INFO "; \
            break; \
        case DPLevel::DP_VERBOSE: \
            std::clog << "VERBOSE "; \
            break; \
        case DPLevel::DP_DEBUG: \
            std::clog << "DEBUG "; \
            break; \
        default: \
            std::clog << "??? "; \
            break; \
        } \
        DebugPrint::printMsgs(__VA_ARGS__); \
    } while (false);
#else
#define SS_DPRINT(type, ...) \
    if constexpr (type == DPLevel::DP_ERROR) { \
        std::clog << __FILE__ << ':' << __LINE__ << ' '; \
        std::clog << "ERROR "; \
        DebugPrint::printMsgs(__VA_ARGS__); \
    } else if constexpr (type == DPLevel::DP_WARNING) { \
        std::clog << __FILE__ << ':' << __LINE__ << ' '; \
        std::clog << "WARNING "; \
        DebugPrint::printMsgs(__VA_ARGS__); \
    }
#endif

namespace DebugPrint {
    template <typename Arg>
    void printMsgs(Arg arg) {
        std::clog << arg << '\n';
    }

    template <typename Arg, typename ...Args>
    void printMsgs(Arg arg, Args ...args){
        std::clog << arg;
        printMsgs(args...);
    }
} // namespace DebugPrint

#endif
