#pragma once

#include <SafeQueue.hpp>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>

class Logger {
public:
  Logger(const std::string &filename);
  ~Logger();
  template <typename... Args>
  auto log(const std::string &format_str, Args &&...args) -> void;
  template <typename... Args>
  auto format(const std::string &format_str, Args &&...args) -> std::string;
  auto process() -> void;
  auto shutdown() -> void;

private:
  SafeQueue<std::string> log_queue_;
  std::thread worker_;
  std::ofstream log_file_;
};

inline Logger::Logger(const std::string &filename)
    : log_queue_(10), log_file_(filename, std::ios::out | std::ios::app) {
  if (!log_file_.is_open()) {
    throw std::runtime_error("Failed to Open the log file.");
  }
  worker_ = std::thread(&Logger::process, this);
}

inline Logger::~Logger() {
  this->shutdown();
  if (this->worker_.joinable()) {
    this->worker_.join();
  }
}

template <typename... Args>
auto inline Logger::log(const std::string &format_str, Args &&...args) -> void {
  std::string final_str = format(format_str, std::forward<Args>(args)...);
  log_queue_.enqueue(final_str);
}

template <typename T> std::string to_string_helper(T &&arg) {
  std::ostringstream oss;
  oss << std::forward<T>(arg);
  return oss.str();
}

template <typename... Args>
auto inline Logger::format(const std::string &format_str, Args &&...args)
    -> std::string {
  std::vector<std::string> args_vec = {
      to_string_helper(std::forward<Args>(args))...};

  std::ostringstream res;
  size_t args_idx = 0;
  size_t pos = 0;
  while (true) {
    size_t place_holder_pos = format_str.find("{}", pos);
    if (place_holder_pos == std::string::npos) {
      res << format_str.substr(pos);
      break;
    }
    res << format_str.substr(pos, place_holder_pos - pos);
    if (args_idx < args_vec.size()) {
      res << args_vec[args_idx];
      args_idx++;
    } else {
      res << "{}";
    }
    pos = place_holder_pos + 2;
  }
  return res.str();
}

auto inline Logger::process() -> void {
  std::string element = "";
  while (this->log_queue_.dequeue(element)) {
    this->log_file_ << element << "\n";
  }
}

auto inline Logger::shutdown() -> void { this->log_queue_.shutdown(); }
