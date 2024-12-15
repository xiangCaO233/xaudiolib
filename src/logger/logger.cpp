#include "logger.h"

#include <iostream>
#include <string>

/*
\033[37;41m 设置白色文本（37）和红色背景（41）。
\033[30;43m 设置黑色文本（30）和黄色背景（43）。
\033[34;42m 设置蓝色文本（34）和绿色背景（42）。
\033[0m 用于重置颜色到默认状态。
\033[1m，表示开启加粗
ANSI 颜色代码表
常用的 ANSI 颜色代码：
前景色（文本颜色）：
30 - 黑色
31 - 红色
32 - 绿色
33 - 黄色
34 - 蓝色
35 - 紫色（洋红）
36 - 青色
37 - 白色
背景色：
40 - 黑色
41 - 红色
42 - 绿色
43 - 黄色
44 - 蓝色
45 - 紫色（洋红）
46 - 青色
47 - 白色
*/
void CustomFormatter::format(const spdlog::details::log_msg &msg,
                             spdlog::memory_buf_t &dest) {
    // 使用 std::chrono 格式化时间
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(duration) -
        std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_local = *std::localtime(&time);
    // 使用 fmt::format 格式化时间为 "HH:MM:SS.mmm"
    std::string formatted_time =
        fmt::format("{:02}:{:02}:{:02}.{:03}", tm_local.tm_hour,
                    tm_local.tm_min, tm_local.tm_sec, milliseconds.count());
    // 根据日志等级设置背景色和文本消息的前景色
    std::string bg_color_code;
    std::string message_color_code;
    std::string message_forge_color_code;
    std::string log_level_str;
    bool should_show_file = false;
    switch (msg.level) {
        case spdlog::level::critical:
            // 黑背景
            bg_color_code = "\033[40m";
            // 红背景 + 蓝前景
            message_color_code = "\033[34;41m";
            message_forge_color_code = "\033[1;31;40m";
            log_level_str = "CRITICAL";
            should_show_file = true;
            break;
        case spdlog::level::err:
            bg_color_code = "\033[40m";
            message_color_code = "\033[37;41m";
            message_forge_color_code = "\033[1;31;40m";
            log_level_str = "ERROR";
            should_show_file = true;
            break;
        case spdlog::level::warn:
            bg_color_code = "\033[40m";
            // 黄背景+黑前景
            message_color_code = "\033[0;30;43m";
            message_forge_color_code = "\033[1;33;40m";
            log_level_str = "WARN";
            should_show_file = true;
            break;
        case spdlog::level::info:
            bg_color_code = "\033[40m";
            // 黑色前景+绿色背景
            message_color_code = "\033[0;30;42m";
            message_forge_color_code = "\033[1;32;40m";
            log_level_str = "INFO";
            break;
        case spdlog::level::debug:
            bg_color_code = "\033[40m";
            // 蓝色背景 + 白色前景
            message_color_code = "\033[0;97;44m";
            message_forge_color_code = "\033[1;34;40m";
            log_level_str = "DEBUG";
            should_show_file = true;
            break;
        case spdlog::level::trace:
            bg_color_code = "\033[40m";            // 白色背景
            message_color_code = "\033[0;30;47m";  // 黑色前景
            message_forge_color_code = "\033[1;37;40m";
            log_level_str = "TRACE";
            break;
        default:
            bg_color_code = "\033[40m";          // 黑色背景
            message_color_code = "\033[97;40m";  // 白色前景
            break;
    }
    // 文件名和行号：紫色不加粗，若背景是紫色，则切换为白色
    std::string file_color_code =
        (bg_color_code == "\033[45m") ? "\033[97m" : "\033[35m";
    // 函数名：绿色加粗
    std::string function_color_code = "\033[32;1m";
    // 写入时间和背景色
    fmt::format_to(std::back_inserter(dest), "{}\033[36;1m[{}] \033[0m",
                   bg_color_code, formatted_time);
    // 添加日志等级，使用背景色当做前景色
    fmt::format_to(std::back_inserter(dest),
                   "\033[1;97;40m[{}{}\033[1;97;40m/{}",
                   message_forge_color_code, log_level_str, bg_color_code);
    // 添加函数名
    fmt::format_to(std::back_inserter(dest), "{}{}{}\033[1;97;40m]: {}",
                   bg_color_code, function_color_code, msg.source.funcname,
                   bg_color_code);
    // 文本消息
    fmt::format_to(std::back_inserter(dest), "{}{}{}", message_color_code,
                   msg.payload, bg_color_code);
    if (should_show_file) {
        // 添加文件和行号
        fmt::format_to(std::back_inserter(dest), "{} [{}:{}]{}",
                       file_color_code, msg.source.filename, msg.source.line,
                       bg_color_code);
    }
    fmt::format_to(std::back_inserter(dest), "{}{}{}", bg_color_code, "\n",
                   bg_color_code);
};

std::unique_ptr<spdlog::formatter> CustomFormatter::clone() const {
    return std::make_unique<CustomFormatter>(*this);
};

std::shared_ptr<spdlog::logger> XLogger::_logger;

// 包装的日志函数，带有调用者信息
void XLogger::log(spdlog::level::level_enum level,
                  const spdlog::source_loc &loc, const std::string &msg) {
    _logger->log(loc, level, msg);
};

void XLogger::init() {
    if (!setlocale(LC_ALL, "en_US.UTF-8")) {
        std::cerr << "修改区域设置为 en_US.UTF-8 失败" << std::endl;
    }
    // 创建一个控制台日志 sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);
    // 创建一个文件日志 sink
    auto file_sink =
        std::make_shared<spdlog::sinks::basic_file_sink_mt>("debug.log", true);
    file_sink->set_level(spdlog::level::trace);
    // 设置异步日志器，结合两个 sink
    spdlog::init_thread_pool(1, 1);
    _logger = std::make_shared<spdlog::async_logger>(
        "async_logger", spdlog::sinks_init_list{console_sink, file_sink},
        spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    // 给控制台日志器使用自定义格式化器
    auto formatter = std::make_unique<CustomFormatter>();
    console_sink->set_formatter(std::move(formatter));
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#]: %v");
    // 设置日志等级
    _logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(_logger);
};

// 包装器的日志类型函数
void XLogger::trace(const spdlog::source_loc &loc, const std::string &msg) {
    log(spdlog::level::trace, loc, msg);
};

void XLogger::debug(const spdlog::source_loc &loc, const std::string &msg) {
    log(spdlog::level::debug, loc, msg);
};

void XLogger::info(const spdlog::source_loc &loc, const std::string &msg) {
    log(spdlog::level::info, loc, msg);
};

void XLogger::warn(const spdlog::source_loc &loc, const std::string &msg) {
    log(spdlog::level::warn, loc, msg);
};

void XLogger::error(const spdlog::source_loc &loc, const std::string &msg) {
    log(spdlog::level::err, loc, msg);
};

void XLogger::critical(const spdlog::source_loc &loc, const std::string &msg) {
    log(spdlog::level::critical, loc, msg);
};
