/*
 * Copyright (C) 2013-2014, The OpenFlint Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS-IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sys/errno.h>
#include "Logging.h"

using namespace flint;

namespace  flint {

const char *log_levelName[Logger::NUM_LOG_LEVELS] = {
        "\033[;37m" "TRACE ",
        "\033[;34m" "DEBUG ",
        "\033[;32m" "INFO  ",
        "\033[;33m" "WARN  ",
        "\033[;35m" "ERROR ",
        "\033[;31m" "FATAL ",
};

// helper class for known string length at compile time
class T {
public:
    T(const char *str, unsigned len)
            : str_(str),
              len_(len) {
        assert(strlen(str) == len_);
    }

    const char *str_;
    const unsigned len_;
};

inline LogStream &operator<<(LogStream &s, T v) {
    s.append(v.str_, v.len_);
    return s;
}

inline LogStream &operator<<(LogStream &s, const Logger::SourceFile &v) {
    s.append(v.data_, v.size_);
    return s;
}

}

Logger::Impl::Impl(log_level level, const SourceFile &file, int line)
        : stream_(),
          level_(level),
          line_(line),
          basename_(file) {
    stream_ << T(log_levelName[level], 12);
}

void Logger::Impl::finish() {
    stream_ << " - " << basename_ << ':' << line_ << "\033[0m" << '\n';
}

Logger::Logger(SourceFile file, int line)
        : impl_(INFO, file, line) {
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char *func)
        : impl_(level, file, line) {
    impl_.stream_ << func << ' ';
}

Logger::Logger(SourceFile file, int line, LogLevel level)
        : impl_(level, file, line) {
}

Logger::Logger(SourceFile file, int line, bool to_abort)
        : impl_(to_abort ? FATAL : ERROR, file, line) {
}

Logger::~Logger() {
    impl_.finish();
    const LogStream::LogBuffer &buf(stream().buffer());

    fwrite(buf.data(), 1, (size_t) buf.length(), stdout);

    if (impl_.level_ == FATAL) {
        fflush(stdout);
        abort();
    }
}
