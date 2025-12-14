#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "uring_aio.h"

namespace shlog {

using LogMessage = std::string;

class LogSinkBase {
   public:
    LogSinkBase() = default;
    virtual ~LogSinkBase() = default;

    virtual void log(LogMessage&) = 0;
    virtual void flush() = 0;
};

class FileSinkBase : public LogSinkBase {
   public:
    FileSinkBase(const std::string& path = "", bool append = false);
    virtual ~FileSinkBase();

    virtual void open(const std::string& file_path, bool append = false);
    virtual void close();

    int fd() const { return fd_; }
    const std::string& path() const { return path_; }

   protected:
    std::string defaultFilePath();

    std::string path_;
    int fd_{-1};
    off_t offset_{-1};
};

class StandardFileSink : public FileSinkBase {
   public:
    StandardFileSink() = default;
    ~StandardFileSink();

    virtual void log(LogMessage&) override;
    virtual void flush() override;
};

class UringFileSink : public FileSinkBase {
   public:
    UringFileSink();
    ~UringFileSink();

    virtual void log(LogMessage&) override;
    virtual void flush() override;

    UringAIO<SQ_POLL::ENABLED, FD_FIXED::YES> aio_;
};

class ConsoleSink : public LogSinkBase {
   public:
    ~ConsoleSink() { flush(); }

    void log(LogMessage& msg) override { std::cout << msg.data(); }
    void flush() override { fflush(stdout); }
};

using SinkPtr = std::unique_ptr<LogSinkBase>;
}  // namespace shlog