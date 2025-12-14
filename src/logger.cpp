#include "logger.h"

namespace shlog {

Logger::~Logger() {
    stop_ = true;
    if (processThread_.joinable()) {
        processThread_.join();
    }
}

void Logger::init(LogLevel level, SinkPtr sink) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (processThread_.joinable()) {
        processThread_.join();
    }

    setLogLevel(level);
    setLogSink(std::move(sink));

    stop_.store(false);
    // Start the log processing thread
    processThread_ = std::thread([this] { processLogTasks(); });
}

void Logger::processLogTasks() {
    while (true) {
        if (stop_ && taskQueue_.empty()) {
            break;
        }
        std::function<void()> task{nullptr};
        if (!taskQueue_.empty()) {
            taskQueue_.pop(task);
            if (task) task();
        }
    }
}
}  // namespace shlog
