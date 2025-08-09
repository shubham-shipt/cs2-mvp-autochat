#pragma once
#include <cstdio>
#include <string>
#include <ctime>
#include <mutex>
#include <fstream>
#include <sstream>
namespace logger {
enum class Level { DEBUG, INFO, WARN, ERROR };
class Logger {
public:
    Logger(const std::string& filename = "logs/app.log", size_t max_size = 1024 * 1024, int max_files = 3)
        : file_(filename), max_size_(max_size), max_files_(max_files) {}
    void log(Level lvl, const std::string& msg) {
        std::lock_guard<std::mutex> lock(mtx_);
        rotate_if_needed();
        std::ofstream out(file_, std::ios::app);
        out << "[" << time_str() << "][" << level_str(lvl) << "] " << msg << std::endl;
        if (lvl >= Level::ERROR) std::fprintf(stderr, "[%s][%s] %s\n", time_str().c_str(), level_str(lvl).c_str(), msg.c_str());
        else std::printf("[%s][%s] %s\n", time_str().c_str(), level_str(lvl).c_str(), msg.c_str());
    }
    void debug(const std::string& msg)  { log(Level::DEBUG, msg); }
    void info(const std::string& msg)   { log(Level::INFO, msg); }
    void warn(const std::string& msg)   { log(Level::WARN, msg); }
    void error(const std::string& msg)  { log(Level::ERROR, msg); }
    static std::string level_str(Level lvl) {
        switch (lvl) {
            case Level::DEBUG: return "DBG";
            case Level::INFO:  return "INF";
            case Level::WARN:  return "WRN";
            case Level::ERROR: return "ERR";
        }
        return "UNK";
    }
private:
    void rotate_if_needed() {
        std::ifstream in(file_, std::ios::ate | std::ios::binary);
        if (!in) return;
        size_t len = in.tellg();
        if (len < max_size_) return;
        in.close();
        for (int i = max_files_ - 1; i > 0; --i) {
            std::string old_file = file_ + "." + std::to_string(i - 1);
            std::string new_file = file_ + "." + std::to_string(i);
            std::ifstream src(i == 1 ? file_ : old_file, std::ios::binary);
            std::ofstream dst(new_file, std::ios::binary);
            if (src && dst) dst << src.rdbuf();
        }
        std::ofstream trunc(file_, std::ios::trunc); // clear current log
    }
    std::string time_str() {
        std::time_t t = std::time(nullptr);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%F %T", std::localtime(&t));
        return buf;
    }
    std::string file_;
    size_t max_size_;
    int max_files_;
    std::mutex mtx_;
};
inline Logger& default_logger() {
    static Logger logger;
    return logger;
}
#define LOGD(msg) ::logger::default_logger().debug(msg)
#define LOGI(msg) ::logger::default_logger().info(msg)
#define LOGW(msg) ::logger::default_logger().warn(msg)
#define LOGE(msg) ::logger::default_logger().error(msg)
}