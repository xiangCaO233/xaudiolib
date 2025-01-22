#include <glog/logging.h>

#include "colorful-log.h"

void ColorfulLogSink::setFunctionName(const std::string& func) {
  current_function_ = func;
}

void ColorfulLogSink::send(google::LogSeverity severity, const char* filename,
                           const char* base_filename, int line,
                           const struct tm* tm_time, const char* message,
                           size_t message_len) {
  // 颜色代码
  const std::string RESET = "\033[0m";
  const std::string BOLD = "\033[1m";
  const std::string BLACK_BG = "\033[40m";

  const std::string CYAN_FG = "\033[36m";
  const std::string GREEN_FG = "\033[32m";
  const std::string YELLOW_FG = "\033[33m";
  const std::string RED_FG = "\033[31m";
  const std::string WHITE_FG = "\033[37m";
  const std::string PURPLE_FG = "\033[35m";

  // 时间格式化
  char time_buffer[128];
  strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", tm_time);

  // 日志等级样式
  std::string severity_style;
  std::string message_style;
  switch (severity) {
    case google::INFO:
      severity_style = BLACK_BG + GREEN_FG + BOLD;
      message_style = severity_style;
      break;
    case google::WARNING:
      severity_style = BLACK_BG + YELLOW_FG + BOLD;
      message_style = severity_style;
      break;
    case google::ERROR:
      severity_style = BLACK_BG + RED_FG + BOLD;
      message_style = severity_style;
      break;
    case google::FATAL:
      severity_style = RED_FG + BLACK_BG + BOLD;
      message_style = severity_style;
      break;
    default:
      severity_style = BLACK_BG + WHITE_FG + BOLD;
      message_style = severity_style;
      break;
  }

  // 输出格式
  std::cerr << BLACK_BG + WHITE_FG + BOLD << "[" << CYAN_FG << time_buffer
            << WHITE_FG << "]" << " " << "[" << severity_style
            << (severity == google::INFO      ? "INFO"
                : severity == google::WARNING ? "WARN"
                : severity == google::ERROR   ? "ERROR"
                                              : "FATAL")
            << WHITE_FG << "/" << PURPLE_FG << current_function_ << WHITE_FG
            << "]" << ": " << RESET << message_style
            << std::string(message, message_len) << " " << RESET;

  // 如果是 WARNING 或以上级别，附加文件名和行号
  if (severity >= google::WARNING) {
    std::cerr << BLACK_BG + PURPLE_FG + BOLD << "[" << base_filename << ":"
              << line << "]" << RESET;
  }

  std::cerr << std::endl;
}

ColorfulLogSink* GLogger::sink;

// 初始化GLogger
void GLogger::init(const char* name) {
  // 初始化glog
  FLAGS_logtostderr = false;
  FLAGS_alsologtostderr = false;
  FLAGS_logtostdout = false;
  FLAGS_colorlogtostdout = false;
  FLAGS_colorlogtostderr = false;
  FLAGS_log_dir = "";
  FLAGS_stderrthreshold = 5;

  FLAGS_minloglevel = 0;
  google::InitGoogleLogging(name);

  // 捕获 SIGSEGV 等信号
  google::InstallFailureSignalHandler();

  sink = new ColorfulLogSink;
  // 安装日志 sink
  google::AddLogSink(sink);
}
void GLogger::destroy() {
  google::RemoveLogSink(sink);
  delete sink;
};
