#include "shlog/logger.h"

namespace shlog {

MTLogger::MTLogger() { stop_.store(true); }

MTLogger::~MTLogger() { stop(); }

void MTLogger::stop() {
    stop_.store(true);
    if (processThread_.joinable()) {
        processThread_.join();
    }
}

void MTLogger::init(LogLevel level, SinkPtr sink) {
    std::lock_guard<std::mutex> lock(mutex_);

    stop();

    LoggerBase::init(level, std::move(sink));

    stop_.store(false);
    // Start the log processing thread
    processThread_ = std::thread([this] { processLogTasks(); });
}

void MTLogger::processLogTasks() {
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

STLogger::STLogger() { stop_ = true; }

STLogger::~STLogger() { stop(); }

void STLogger::stop() {
    stop_ = true;
    if (processThread_.joinable()) {
        processThread_.join();
    }
}

void STLogger::init(LogLevel level, SinkPtr sink) {
    stop();

    LoggerBase::init(level, std::move(sink));
    stop_ = false;
    // Start the log processing thread
    processThread_ = std::thread([this] { processLogTasks(); });
}

void STLogger::processLogTasks() {
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
