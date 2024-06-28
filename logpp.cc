#include <cstdio>
#include <logpp.hh>
#include <print>
#include <ranges>
#include <unordered_map>
namespace {
static std::unordered_map<LogPP::LogLevel, const char *> style_map{
  {LogPP::LogLevel::trace, "\x1b[2;37m"},
  {LogPP::LogLevel::debug, "\x1b[37m"},
  {LogPP::LogLevel::information, "\x1b[36m"},
  {LogPP::LogLevel::warning, "\x1b[33m"},
  {LogPP::LogLevel::error, "\x1b[1;35m"},
  {LogPP::LogLevel::critical, "\x1b[1;4;5;31m"},
};
static std::unordered_map<LogPP::LogLevel, char> tag_map{
  {LogPP::LogLevel::trace, 'T'},
  {LogPP::LogLevel::debug, 'D'},
  {LogPP::LogLevel::information, 'I'},
  {LogPP::LogLevel::warning, 'W'},
  {LogPP::LogLevel::error, 'E'},
  {LogPP::LogLevel::critical, 'C'},
};
static void
write_log(LogPP::LogLevel level, std::vector<std::string> &chain, std::string &&time, std::string &&content) {
  std::string prefix = style_map[level];
  prefix.push_back('[');
  prefix.push_back(tag_map[level]);
  prefix.push_back(']');
  prefix.append(time);
  if (!chain.empty()) {
    bool start = true;
    prefix.push_back('[');
    for (const auto &name : std::ranges::reverse_view(chain)) {
      if (start) {
        start = false;
      } else {
        prefix.append("/");
      }
      prefix.append(name);
    }
    prefix.push_back(']');
  }
  prefix.push_back(' ');
  std::string suffix = "\x1b[0m";
  std::string buffer;
  size_t      start = 0;
  while (true) {
    auto find = content.find('\n', start);
    if (find == content.npos) {
      if (start < content.size()) {
        buffer.append(std::format("{}{}{}\n", prefix, content.substr(start), suffix));
      }
      break;
    }
    buffer.append(std::format("{}{}{}\n", prefix, content.substr(start, find - start), suffix));
    start = find + 1;
  }
  std::print("{}", buffer);
  ::fflush(stdout);
}
}; // namespace
void LogPP::Logger::do_log(
  LogLevel level, std::vector<std::string> &chain, std::string &&time, std::string &&content
) const {
  if (level < this->log_level) {
    return;
  }
  if (this->parent_logger == nullptr) {
    return write_log(level, chain, std::move(time), std::move(content));
  }
  chain.emplace_back(this->name);
  this->parent_logger->do_log(level, chain, std::move(time), std::move(content));
}
LogPP::Logger::Logger() : log_level(LogLevel::information), parent_logger(nullptr), name("") {}
LogPP::Logger::Logger(const Logger *const parent, std::string_view name)
  : log_level(parent->log_level), parent_logger(parent), name(name) {}

LogPP::Logger &LogPP::Logger::get_global_logger() {
  static Logger logger{};
  return logger;
}

[[nodiscard]] LogPP::Logger LogPP::Logger::create_sub_logger(std::string_view name) const {
  return {this, name};
}
void LogPP::Logger::set_log_level(LogLevel new_level) { this->log_level = new_level; }

LogPP::Logger &LogPP::logger = Logger::get_global_logger();