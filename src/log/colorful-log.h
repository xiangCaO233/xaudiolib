#ifndef XIANG_COLORFULLOG_H
#define XIANG_COLORFULLOG_H

#include <glog/logging.h>

// 定义日志宏(传递函数名)

#define LOGINFO(msg)                            \
  GLogger::sink->setFunctionName(__FUNCTION__); \
  LOG(INFO) << msg;

#define LOGWARN(msg)                            \
  GLogger::sink->setFunctionName(__FUNCTION__); \
  LOG(WARNING) << msg;

#define LOGERROR(msg)                           \
  GLogger::sink->setFunctionName(__FUNCTION__); \
  LOG(ERROR) << msg;

#define LOGFATAL(msg)                           \
  GLogger::sink->setFunctionName(__FUNCTION__); \
  LOG(FATAL) << msg;

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
