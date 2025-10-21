#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>

class Logger
{
public:
    // Define ANSI color codes as constants
    static constexpr const char RESET[] = "\033[0m";
    static constexpr const char BLACK[] = "\033[30m";
    static constexpr const char RED[] = "\033[31m"; // Error
    static constexpr const char GREEN[] = "\033[32m"; // Success
    static constexpr const char YELLOW[] = "\033[33m"; // Warning (approximation for orange)
    static constexpr const char BLUE[] = "\033[34m"; // Info
    static constexpr const char MAGENTA[] = "\033[35m"; // Alternative
    static constexpr const char CYAN[] = "\033[36m"; // Additional
    static constexpr const char WHITE[] = "\033[37m"; // Default white
    static constexpr const char GRAY[] = "\033[90m"; // Gray
    static constexpr const char BRIGHT_RED[] = "\033[91m"; // Bright error
    static constexpr const char BRIGHT_GREEN[] = "\033[92m"; // Bright success

    // Whether Logger should be active or not
    inline static bool enabled = true;

    static void disable() { enabled = false; }
    static void enable() { enabled = true; }

    inline static void black(std::string msg, std::ostream& os = std::cout)
    {
        if (!enabled) return;
        os << BLACK << "[BLACK] " << RESET
            << msg
            << std::endl;
    }

    inline static void error(std::string msg, std::ostream& os = std::cout)
    {
        if (!enabled) return;
        os << RED << "[ERROR] " << RESET
            << msg
            << std::endl;
    }

    inline static void success(std::string msg, std::ostream& os = std::cout)
    {
        if (!enabled) return;
        os << GREEN << "[SUCCESS] " << RESET
            << msg
            << std::endl;
    }

    inline static void warning(std::string msg, std::ostream& os = std::cout)
    {
        if (!enabled) return;
        os << YELLOW << "[WARNING] " << RESET
            << msg
            << std::endl;
    }

    inline static void info(std::string msg, std::ostream& os = std::cout)
    {
        if (!enabled) return;
        os << BLUE << "[INFO] " << RESET
            << msg
            << std::endl;
    }

    inline static void log(std::string msg, std::ostream& os = std::cout)
    {
        if (!enabled) return;
        os << GRAY << "[-] "
            << msg << RESET
            << std::endl;
    }

    inline static void magenta(std::string msg, std::ostream& os = std::cout)
    {
        if (!enabled) return;
        os << MAGENTA << "[MAGENTA] " << RESET
            << msg
            << std::endl;
    }

    inline static void cyan(std::string msg, std::ostream& os = std::cout)
    {
        if (!enabled) return;
        os << CYAN << "[CYAN] " << RESET
            << msg
            << std::endl;
    }

    inline static void white(std::string msg, std::ostream& os = std::cout)
    {
        if (!enabled) return;
        os << WHITE << "[WHITE] " << RESET
            << msg
            << std::endl;
    }

    inline static void note(std::string msg, std::ostream& os = std::cout)
    {
        if (!enabled) return;
        os << GRAY << "[NOTE] "
            << msg << RESET
            << std::endl;
    }

    inline static void brightError(std::string msg, std::ostream& os = std::cout)
    {
        if (!enabled) return;
        os << BRIGHT_RED << "[BRIGHT ERROR] "
            << msg << RESET
            << std::endl;
    }

    inline static void brightSuccess(std::string msg, std::ostream& os = std::cout)
    {
        if (!enabled) return;
        os << BRIGHT_GREEN << "[BRIGHT SUCCESS] "
            << msg << RESET
            << std::endl;
    }
};

#endif