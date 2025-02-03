#ifndef XIANG_COLORFULLOG_H
#define XIANG_COLORFULLOG_H

#include <spdlog/common.h>
#ifdef _WIN32
#define SPDLOG_FUNCTION __FUNCTION__
#endif

#include <spdlog/details/log_msg.h>
#include <spdlog/fmt/chrono.h>
#include <spdlog/formatter.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>

#define XTRACE(msg) SPDLOG_TRACE(msg)
#define XDEBUG(msg) SPDLOG_DEBUG(msg)
#define XINFO(msg) SPDLOG_INFO(msg)
#define XWARN(msg) SPDLOG_WARN(msg)
#define XERROR(msg) SPDLOG_ERROR(msg)
#define XCRITICAL(msg) SPDLOG_CRITICAL(msg)

class ColorfulFormatter : public spdlog::formatter {
 public:
  void format(const spdlog::details::log_msg& msg,
              spdlog::memory_buf_t& dest) override;
  std::unique_ptr<spdlog::formatter> clone() const override;

 private:
  const char* get_color(spdlog::level::level_enum level) const;
};

class XLogger {
  // 日志实体
  static std::shared_ptr<spdlog::logger> logger;

 public:
  static void init();
  static void shutdown();
  static void enable();
  static void disable();
  static void setlevel(spdlog::level::level_enum level);
};
#endif  // XIANG_COLORFULLOG_H
