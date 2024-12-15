#ifndef X_LOGGER_H
#define X_LOGGER_H

#include <fmt/core.h>
#include <spdlog/async.h>
#include <spdlog/common.h>
#include <spdlog/details/fmt_helper.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/formatter.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <ctime>
#include <memory>
#include <string>

#ifdef _WIN32
#include <windows.h>
#elif __unix__ || __APPLE__

#include <sys/ioctl.h>
#include <unistd.h>

#endif

class CustomFormatter : public spdlog::formatter {
   public:
    void format(const spdlog::details::log_msg &msg,
                spdlog::memory_buf_t &dest) override;

    std::unique_ptr<spdlog::formatter> clone() const override;
};

class XLogger {
    static std::shared_ptr<spdlog::logger> _logger;

    // 自定义每个日志级别的颜色代码
    static std::string get_level_color(spdlog::level::level_enum level);

    // 包装的日志函数，带有调用者信息
    static void log(spdlog::level::level_enum level,
                    const spdlog::source_loc &loc, const std::string &msg);

   public:
    // 获取文件名
    inline static const char *get_relative_file_path(const char *full_path) {
        if (!full_path || *full_path == '\0') {
            return "unknown";
        }

        static thread_local std::string relative_path;
        std::string path(full_path);
        size_t pos;
#ifdef _WIN32
        pos = path.find_last_of("\\");
#else
        pos = path.find_last_of("/\\");
#endif
        if (pos != std::string::npos) {
            relative_path = path.substr(pos + 1);
        } else {
            relative_path = path;
        }
        return relative_path.c_str();
    }

    static void init();

    // 包装器的日志类型函数
    static void trace(const spdlog::source_loc &loc, const std::string &msg);

    static void debug(const spdlog::source_loc &loc, const std::string &msg);

    static void info(const spdlog::source_loc &loc, const std::string &msg);

    static void warn(const spdlog::source_loc &loc, const std::string &msg);

    static void error(const spdlog::source_loc &loc, const std::string &msg);

    static void critical(const spdlog::source_loc &loc, const std::string &msg);
};

// 定义自定义的 LOG_LOC 宏
#define LOG_LOC                                                           \
    spdlog::source_loc {                                                  \
        XLogger::get_relative_file_path(__FILE__), __LINE__, __FUNCTION__ \
    }
// 每种日志类型的宏，用于自动捕获调用者的信息
#define LOG_TRACE(msg) XLogger::trace(LOG_LOC, msg)
#define LOG_DEBUG(msg) XLogger::debug(LOG_LOC, msg)
#define LOG_INFO(msg) XLogger::info(LOG_LOC, msg)
#define LOG_WARN(msg) XLogger::warn(LOG_LOC, msg)
#define LOG_ERROR(msg) XLogger::error(LOG_LOC, msg)
#define LOG_CRITICAL(msg) XLogger::critical(LOG_LOC, msg)

#endif /* X_LOGGER_H */
