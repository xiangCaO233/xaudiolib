#ifndef XIANG_COLORFULLOG_H
#define XIANG_COLORFULLOG_H

#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>

class XLogger {
  // 日志实体
  static std::shared_ptr<spdlog::logger> logger;

 public:
  static void init();
};
#endif  // XIANG_COLORFULLOG_H
