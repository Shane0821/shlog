#ifndef _LOGGER_H
#define _LOGGER_H

#include <fmt/core.h>

#include <atomic>
#include <ctime>
#include <functional>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#include "log_sink.h"
#include "mpmc_queue.hpp"
#include "singleton.hpp"

class Logger : public Singleton<Logger> {
    friend class Singleton<Logger>;

   public:
    enum class LogLevel { TRACE, DEBUG, INFO, WARNING, ERROR, FATAL };

    ~Logger();

    // Initialize logger with output file
    void init(LogLevel level = LogLevel::INFO,
              std::unique_ptr<LogSinkBase> = std::make_unique<ConsoleSink>());

    // add a log task to the queue
    template <LogLevel Level, typename... Args>
    void log(int line, const std::string& format, Args&&... args) {
        if (Level < level_) return;

        if (stop_) return;

        taskQueue_.emplace(
            [... args = std::forward<Args>(args), line, format, this]() mutable {
                auto pid = std::this_thread::get_id();
                auto logLine = fmt::format("[{}][{}][{}][{}:{}]: {}\n", *(size_t*)&pid,
                                           time(NULL), levelToString<Level>(), __FILE__,
                                           line, fmt::format(format, std::move(args)...));
                sink_->log(logLine);
            });
    }

    void setLogLevel(LogLevel level) { level_ = level; }
    Logger::LogLevel getLogLevel() const { return level_; }

    void setLogSink(std::unique_ptr<LogSinkBase> sink) { sink_ = std::move(sink); }

   protected:
    Logger() = default;

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

   private:
    template <LogLevel level>
    constexpr const char* levelToString() {
        if constexpr (level == LogLevel::TRACE) {
            return "TRACE";
        } else if constexpr (level == LogLevel::DEBUG) {
            return "DEBUG";
        } else if constexpr (level == LogLevel::INFO) {
            return "INFO";
        } else if constexpr (level == LogLevel::WARNING) {
            return "WARNING";
        } else if constexpr (level == LogLevel::ERROR) {
            return "ERROR";
        } else if constexpr (level == LogLevel::FATAL) {
            return "FATAL";
        } else {
            static_assert(level != level, "Unknown log level");
            return "UNKNOWN";
        }
    }

    // Process log tasks
    void processLogTasks();

    std::unique_ptr<LogSinkBase> sink_;
    std::mutex mutex_;
    MPMCQueue<std::function<void()>> taskQueue_;
    std::thread processThread_;
    std::atomic<bool> stop_;
    LogLevel level_;
};

#define LOG_INIT(level, ...) Logger::GetInst().init(level, ##__VA_ARGS__)

#define LOG_TRACE(format, ...) \
    Logger::GetInst().log<Logger::LogLevel::TRACE>(__LINE__, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) \
    Logger::GetInst().log<Logger::LogLevel::DEBUG>(__LINE__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) \
    Logger::GetInst().log<Logger::LogLevel::INFO>(__LINE__, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) \
    Logger::GetInst().log<Logger::LogLevel::WARNING>(__LINE__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) \
    Logger::GetInst().log<Logger::LogLevel::ERROR>(__LINE__, format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) \
    Logger::GetInst().log<Logger::LogLevel::FATAL>(__LINE__, format, ##__VA_ARGS__)

#endif  // _RPC_LOGGER_H