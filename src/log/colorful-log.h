#ifndef XIANG_COLORFULLOG_H
#define XIANG_COLORFULLOG_H

#include <glog/logging.h>

#include <iostream>

// 自定义日志宏
#define LOGINFO                                                            \
  [&](const std::string& func_name) {                                      \
    GLogger::sink->setFunctionName(func_name);                             \
    return google::LogMessage("", __LINE__, google::GLOG_##INFO).stream(); \
  }(__FUNCTION__)

#define LOGWARN                                                               \
  [&](const std::string& func_name) {                                         \
    GLogger::sink->setFunctionName(func_name);                                \
    return google::LogMessage("", __LINE__, google::GLOG_##WARNING).stream(); \
  }(__FUNCTION__)

#define LOGERROR                                                            \
  [&](const std::string& func_name) {                                       \
    GLogger::sink->setFunctionName(func_name);                              \
    return google::LogMessage("", __LINE__, google::GLOG_##ERROR).stream(); \
  }(__FUNCTION__)

#define LOGFATAL                                                            \
  [&](const std::string& func_name) {                                       \
    GLogger::sink->setFunctionName(func_name);                              \
    return google::LogMessage("", __LINE__, google::GLOG_##FATAL).stream(); \
  }(__FUNCTION__)

class ColorfulLogSink : public google::LogSink {
  std::string current_function_;

 public:
  void send(google::LogSeverity severity, const char* filename,
            const char* base_filename, int line, const struct tm* tm_time,
            const char* message, size_t message_len) override;
  void setFunctionName(const std::string& func);
};

class GLogger {
 public:
  // 初始化GLogger
  static void init(const char* name);
  static void destroy();

  static ColorfulLogSink* sink;
};

#endif  // XIANG_COLORFULLOG_H
