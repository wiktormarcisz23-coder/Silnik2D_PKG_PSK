#pragma once
#include <string>
#include <fstream>
#include <mutex>

class Logger {
public:
    static Logger& instance();

    void setLogFile(const std::string& path);
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);

private:
    Logger() = default;
    std::ofstream file_;
    std::mutex mtx_;
    void write(const char* level, const std::string& msg);
};
