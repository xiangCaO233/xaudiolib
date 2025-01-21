#ifndef XIANG_COLORFULLOG_H
#define XIANG_COLORFULLOG_H

#include <glog/logging.h>

class ColorfulLogSink : public google::LogSink {
 public:
  void send(google::LogSeverity severity, const char* filename,
            const char* base_filename, int line, const struct tm* tm_time,
            const char* message, size_t message_len) override;
};

class GLogger {
  static ColorfulLogSink* sink;

 public:
  // 初始化GLogger
  static void init(const char* name);
  static void destroy();
};

#endif  // XIANG_COLORFULLOG_H
