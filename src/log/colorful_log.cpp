#include <memory>

#include "colorful-log.h"

std::shared_ptr<spdlog::logger> XLogger::logger;

void XLogger::init() {
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      "logs/logs.log", true);

  std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
  logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(),
                                            sinks.end());
  spdlog::register_logger(logger);
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
  logger->info("日志初始化完成");
}
