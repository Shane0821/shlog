#include "logger.h"

#include <gtest/gtest.h>

static size_t write_count = 1 << 19;

class Timer {
   private:
    // Type aliases to make accessing nested type easier
    using Clock = std::chrono::steady_clock;
    using Second = std::chrono::duration<double, std::ratio<1> >;

    std::chrono::time_point<Clock> m_beg{Clock::now()};

   public:
    void reset() { m_beg = Clock::now(); }

    double elapsed() const {
        return std::chrono::duration_cast<Second>(Clock::now() - m_beg).count();
    }
};

TEST(STLoggerTest, ConsoleSink) {
    SHLOG_INIT(shlog::LogLevel::DEBUG);
    for (size_t i = 0; i < write_count; i++) {
        SHLOG_INFO("Console Test INFO: {}", i);
        SHLOG_DEBUG("Console Test DEBUG: {}", i);
        SHLOG_ERROR("Console Test ERROR: {}", i);
    }
}

TEST(STLoggerTest, StandardFileSink) {
    SHLOG_INIT(shlog::LogLevel::DEBUG, std::make_unique<shlog::StandardFileSink>());
    Timer t;
    for (size_t i = 0; i < write_count; i++) {
        SHLOG_INFO("File Test INFO: {}", i);
        SHLOG_DEBUG("File Test DEBUG: {}", i);
        SHLOG_ERROR("File Test ERROR: {}", i);
    }
    std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";
}

TEST(STLoggerTest, UringFileSink) {
    SHLOG_INIT(shlog::LogLevel::DEBUG, std::make_unique<shlog::UringFileSink>());
    Timer t;
    for (size_t i = 0; i < write_count; i++) {
        SHLOG_INFO("SQPoll Test INFO: {}", i);
        SHLOG_DEBUG("SQPoll Test DEBUG: {}", i);
        SHLOG_ERROR("SQPoll Test ERROR: {}", i);
    }
    std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";
}

TEST(MTLoggerTest, ConsoleSink) {
    SHLOG_LOGGER_INIT(shlog::MTLogger, shlog::LogLevel::DEBUG);
    for (size_t i = 0; i < write_count; i++) {
        SHLOG_LOGGER_INFO(shlog::MTLogger, "Console Test INFO: {}", i);
        SHLOG_LOGGER_DEBUG(shlog::MTLogger, "Console Test DEBUG: {}", i);
        SHLOG_LOGGER_ERROR(shlog::MTLogger, "Console Test ERROR: {}", i);
    }
}

TEST(MTLoggerTest, StandardFileSink) {
    SHLOG_LOGGER_INIT(shlog::MTLogger, shlog::LogLevel::DEBUG,
                      std::make_unique<shlog::StandardFileSink>());
    Timer t;
    for (size_t i = 0; i < write_count; i++) {
        SHLOG_LOGGER_INFO(shlog::MTLogger, "File Test INFO: {}", i);
        SHLOG_LOGGER_DEBUG(shlog::MTLogger, "File Test DEBUG: {}", i);
        SHLOG_LOGGER_ERROR(shlog::MTLogger, "File Test ERROR: {}", i);
    }
    std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";
}