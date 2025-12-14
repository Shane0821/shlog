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

TEST(LoggerTest, ConsoleSink) {
    LOG_INIT(Logger::LogLevel::DEBUG);
    for (size_t i = 0; i < write_count; i++) {
        LOG_INFO("SQPoll Test INFO: {}", i);
        LOG_DEBUG("SQPoll Test DEBUG: {}", i);
        LOG_ERROR("SQPoll Test ERROR: {}", i);
    }
}

TEST(LoggerTest, StandardFileSink) {
    LOG_INIT(Logger::LogLevel::DEBUG, std::make_unique<StandardFileSink>());
    Timer t;
    for (size_t i = 0; i < write_count; i++) {
        LOG_INFO("SQPoll Test INFO: {}", i);
        LOG_DEBUG("SQPoll Test DEBUG: {}", i);
        LOG_ERROR("SQPoll Test ERROR: {}", i);
    }
    std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";
}

TEST(LoggerTest, UringFileSink) {
    LOG_INIT(Logger::LogLevel::DEBUG, std::make_unique<UringFileSink>());
    Timer t;
    for (size_t i = 0; i < write_count; i++) {
        LOG_INFO("SQPoll Test INFO: {}", i);
        LOG_DEBUG("SQPoll Test DEBUG: {}", i);
        LOG_ERROR("SQPoll Test ERROR: {}", i);
    }
    std::cout << "Time elapsed: " << t.elapsed() << " seconds\n";
}