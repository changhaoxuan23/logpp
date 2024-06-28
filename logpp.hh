#ifndef LOGPP_HH_
#define LOGPP_HH_
#include <chrono>
#include <cstdint>
#include <format>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
namespace LogPP {
enum LogLevel : uint8_t {
  Full,
  trace,
  debug,
  information,
  warning,
  error,
  critical,
  Silent,
};
class Logger final {
private:
  LogLevel            log_level;
  const Logger *const parent_logger;
  void
  do_log(LogLevel level, std::vector<std::string> &chain, std::string &&time, std::string &&content) const;
  Logger();
  Logger(const Logger *const parent, std::string_view name);

public:
  const std::string name;
  Logger(const Logger &)            = delete;
  Logger(Logger &&)                 = delete;
  Logger &operator=(const Logger &) = delete;
  Logger &operator=(Logger &&)      = delete;

  static Logger &get_global_logger();

  template <typename... Elements>
  inline void
  log(LogLevel level, std::format_string<Elements...> format_string, Elements &&...elements) const {
    std::ostringstream buffer;
    auto               clock = std::chrono::system_clock::now();
    auto               time  = std::chrono::system_clock::to_time_t(clock);
    auto               fine  = time_point_cast<std::chrono::milliseconds>(clock);
    buffer << std::put_time(std::localtime(&time), "|%F|%T")
           << std::format(".{:03d}|", fine.time_since_epoch().count() % 1000);
    std::vector<std::string> chain;
    return this->do_log(
      level, chain, buffer.str(), std::format(format_string, std::forward<Elements>(elements)...)
    );
  }
#define LOG_LEVEL_ENTRY(level)                                                                               \
  template <typename... Elements>                                                                            \
  inline void level(std::format_string<Elements...> format_string, Elements &&...elements) const {           \
    return this->log(LogLevel::level, format_string, std::forward<Elements>(elements)...);                   \
  }
  LOG_LEVEL_ENTRY(trace);
  LOG_LEVEL_ENTRY(debug);
  LOG_LEVEL_ENTRY(information);
  LOG_LEVEL_ENTRY(warning);
  LOG_LEVEL_ENTRY(error);
  LOG_LEVEL_ENTRY(critical);

  [[nodiscard]] Logger create_sub_logger(std::string_view name) const;
  void                 set_log_level(LogLevel new_level);
};
extern Logger &logger;
} // namespace LogPP
#endif