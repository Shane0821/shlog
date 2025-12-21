#include "logger.h"

int main() {
    SHLOG_INIT(shlog::LogLevel::DEBUG);
    for (size_t i = 0; i < 4; i++) {
        SHLOG_INFO("Console Test INFO: {}", i);
        SHLOG_DEBUG("Console Test DEBUG: {}", i);
        SHLOG_ERROR("Console Test ERROR: {}", i);
    }
}