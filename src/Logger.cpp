#include "Logger.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

Logger& Logger::instance() {
    static Logger L;
    return L;
}

void Logger::setLogFile(const std::string& path) {
    std::scoped_lock lk(mtx_);
    if (file_.is_open()) file_.close();
    file_.open(path, std::ios::out | std::ios::trunc);
}

void Logger::write(const char* level, const std::string& msg) {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t   = system_clock::to_time_t(now);
    auto ms  = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream ts;
#ifdef _WIN32
    struct tm buf;
    localtime_s(&buf, &t);
    ts << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
#else
    ts << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S");
#endif
    ts << '.' << std::setw(3) << std::setfill('0') << ms.count();

    std::scoped_lock lk(mtx_);
    std::string line = "[" + std::string(level) + "][" + ts.str() + "] " + msg + "\n";

    if (std::string(level) == "ERROR") std::cerr << line;
    else std::cout << line;

    if (file_.is_open()) {
        file_ << line;
        file_.flush();
    }
}

void Logger::info(const std::string& msg) { write("INFO", msg); }
void Logger::warn(const std::string& msg) { write("WARN", msg); }
void Logger::error(const std::string& msg){ write("ERROR", msg); }
