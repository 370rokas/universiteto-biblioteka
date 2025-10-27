#ifndef UB_BACKEND_UTILS_LOGGER
#define UB_BACKEND_UTILS_LOGGER

#include <spdlog/async.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace logger {

inline std::shared_ptr<spdlog::logger> get() {
  auto existing = spdlog::get("backendLogger");
  if (existing)
    return existing;

  // Jei nesukurtas, kuriam nauja loggeri

  if (!spdlog::thread_pool()) {
    spdlog::init_thread_pool(8192, 1);
  }

  auto fileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
      "logs/current.log", 0, 0);
  auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

  std::vector<spdlog::sink_ptr> sinks{consoleSink, fileSink};

  auto logger = std::make_shared<spdlog::async_logger>(
      "backendLogger", sinks.begin(), sinks.end(), spdlog::thread_pool(),
      spdlog::async_overflow_policy::block);

  spdlog::register_logger(logger);
  spdlog::set_default_logger(logger);

  logger->set_level(spdlog::level::debug);
  logger->flush_on(spdlog::level::info);

  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
  spdlog::flush_every(std::chrono::seconds(5));

  return logger;
}

} // namespace logger

#endif // UB_BACKEND_UTILS_LOGGER
