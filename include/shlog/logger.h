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
#include "noncopyable.h"
#include "singleton.hpp"
#include "spsc_queue.hpp"

namespace shlog {

enum class LogLevel { TRACE, DEBUG, INFO, WARNING, ERROR, FATAL, NONE };

class LoggerBase : noncopyable {
   public:
    void init(LogLevel level = LogLevel::INFO,
              SinkPtr sink = std::make_unique<ConsoleSink>()) {
        setLogLevel(level);
        setLogSink(std::move(sink));
    }

    void setLogLevel(LogLevel level) { level_ = level; }
    void setLogSink(SinkPtr sink) { sink_ = std::move(sink); }

   protected:
    LoggerBase() = default;
    ~LoggerBase() = default;

    template <LogLevel level>
    static constexpr const char* levelToString() {
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
            static_assert(level != level, "unknown log level");
            return "UNKNOWN";
        }
    }

    SinkPtr sink_{nullptr};
    LogLevel level_{LogLevel::NONE};
};

class MTLogger : public LoggerBase, public Singleton<MTLogger> {
    friend class Singleton<MTLogger>;

   public:
    ~MTLogger();

    // Initialize logger with output file
    void init(LogLevel level = LogLevel::INFO, SinkPtr = std::make_unique<ConsoleSink>());

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

    void stop();

   protected:
    MTLogger();

    // Process log tasks
    void processLogTasks();

    std::mutex mutex_;
    MPMCQueue<std::function<void()>> taskQueue_;
    std::thread processThread_;
    std::atomic<bool> stop_;
};

class STLogger : public LoggerBase, public Singleton<STLogger> {
    friend class Singleton<STLogger>;

   public:
    ~STLogger();

    // Initialize logger with output file
    void init(LogLevel level = LogLevel::INFO, SinkPtr = std::make_unique<ConsoleSink>());

    // add a log task to the queue
    template <LogLevel Level, typename... Args>
    void log(int line, const std::string& format, Args&&... args) {
        if (Level < level_) return;

        if (stop_) return;

        taskQueue_.emplace([... args = std::forward<Args>(args), line, format,
                            this]() mutable {
            auto logLine =
                fmt::format("[{}][{}][{}:{}]: {}\n", time(NULL), levelToString<Level>(),
                            __FILE__, line, fmt::format(format, std::move(args)...));
            sink_->log(logLine);
        });
    }

    void stop();

   protected:
    STLogger();

    // Process log tasks
    void processLogTasks();

    SPSCQueue<std::function<void()>> taskQueue_;
    std::thread processThread_;
    bool stop_;
};

using DefaultLogger = STLogger;

#endif
}

#define SHLOG_INIT(level, ...) shlog::DefaultLogger::GetInst().init(level, ##__VA_ARGS__)
#define SHLOG_LOGGER_INIT(logger, level, ...) logger::GetInst().init(level, ##__VA_ARGS__)

#define SHLOG_TRACE(format, ...)                                                  \
    shlog::DefaultLogger::GetInst().log<shlog::LogLevel::TRACE>(__LINE__, format, \
                                                                ##__VA_ARGS__)
#define SHLOG_LOGGER_TRACE(logger, format, ...) \
    logger::GetInst().log<shlog::LogLevel::TRACE>(__LINE__, format, ##__VA_ARGS__)

#define SHLOG_DEBUG(format, ...)                                                  \
    shlog::DefaultLogger::GetInst().log<shlog::LogLevel::DEBUG>(__LINE__, format, \
                                                                ##__VA_ARGS__)
#define SHLOG_LOGGER_DEBUG(logger, format, ...) \
    logger::GetInst().log<shlog::LogLevel::DEBUG>(__LINE__, format, ##__VA_ARGS__)

#define SHLOG_INFO(format, ...)                                                  \
    shlog::DefaultLogger::GetInst().log<shlog::LogLevel::INFO>(__LINE__, format, \
                                                               ##__VA_ARGS__)
#define SHLOG_LOGGER_INFO(logger, format, ...) \
    logger::GetInst().log<shlog::LogLevel::INFO>(__LINE__, format, ##__VA_ARGS__)

#define SHLOG_WARN(format, ...)                                                  \
    shlog::DefaultLogger::GetInst().log<shlog::LogLevel::WARN>(__LINE__, format, \
                                                               ##__VA_ARGS__)
#define SHLOG_LOGGER_WARN(logger, format, ...) \
    logger::GetInst().log<shlog::LogLevel::WARN>(__LINE__, format, ##__VA_ARGS__)

#define SHLOG_ERROR(format, ...)                                                  \
    shlog::DefaultLogger::GetInst().log<shlog::LogLevel::ERROR>(__LINE__, format, \
                                                                ##__VA_ARGS__)
#define SHLOG_LOGGER_ERROR(logger, format, ...) \
    logger::GetInst().log<shlog::LogLevel::ERROR>(__LINE__, format, ##__VA_ARGS__)

#define SHLOG_FATAL(format, ...)                                                  \
    shlog::DefaultLogger::GetInst().log<shlog::LogLevel::FATAL>(__LINE__, format, \
                                                                ##__VA_ARGS__)
#define SHLOG_LOGGER_FATAL(logger, format, ...) \
    logger::GetInst().log<shlog::LogLevel::FATAL>(__LINE__, format, ##__VA_ARGS__)
