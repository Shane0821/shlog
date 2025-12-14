#include "log_sink.h"

#include <fcntl.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <system_error>

namespace shlog {
FileSinkBase::FileSinkBase(const std::string& file_path, bool append) {
    open(file_path, append);
}

FileSinkBase::~FileSinkBase() { close(); }

void FileSinkBase::open(const std::string& file_path, bool append) {
    path_ = file_path;

    if (file_path.empty()) {
        path_ = defaultFilePath();
    }

    int flags = O_WRONLY | O_CREAT;
    if (append) {
        flags |= O_APPEND;
    } else {
        flags |= O_TRUNC;
    }

    fd_ = ::open(path_.c_str(), flags, S_IRUSR | S_IWUSR);
    if (fd_ < 0) {
        throw std::system_error(errno, std::system_category(), "failed to open file");
    }

    offset_ = lseek64(fd_, 0, SEEK_END);
    if (offset_ < 0) {
        throw std::system_error(errno, std::system_category(), "failed to seek end");
    }
}

void FileSinkBase::close() {
    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
}

std::string FileSinkBase::defaultFilePath() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S") << ".log";

    return ss.str();
}

// *******************************

StandardFileSink::~StandardFileSink() { flush(); }

void StandardFileSink::log(LogMessage& msg) { ::write(fd_, msg.c_str(), msg.size()); }

void StandardFileSink::flush() { ::fsync(fd_); }

// *******************************

UringFileSink::UringFileSink() { aio_.register_fds(&fd_, 1); }

UringFileSink::~UringFileSink() { flush(); }

void UringFileSink::log(LogMessage& msg) { aio_.write_async(msg, -1, 0); }

void UringFileSink::flush() { aio_.fsync_and_wait(0); }
}