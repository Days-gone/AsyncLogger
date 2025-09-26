#include <Logger.hpp>
#include <gtest/gtest.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>
#include <vector>

namespace {
auto unique_log_file(const std::string &tag) -> std::string {
  auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
  return "logger_" + tag + "_" + std::to_string(stamp) + ".log";
}

auto count_lines(const std::string &path) -> std::size_t {
  std::ifstream in(path);
  std::size_t lines = 0;
  std::string tmp;
  while (std::getline(in, tmp)) {
    ++lines;
  }
  return lines;
}
} // namespace

TEST(LoggerStress, NoDeadlockOnDestruction) {
  const auto path = unique_log_file("deadlock");
  auto start = std::chrono::steady_clock::now();
  {
    Logger logger(path);
    for (int i = 0; i < 5000; ++i) {
      logger.log("Deadlock {}", i);
    }
  }
  auto elapsed = std::chrono::steady_clock::now() - start;
  EXPECT_LT(elapsed, std::chrono::seconds(5));
  std::filesystem::remove(path);
}

TEST(LoggerStress, MultiThreadedHighPressure) {
  const auto path = unique_log_file("multithread");
  constexpr int kThreads = 8;
  constexpr int kMessagesPerThread = 4000;
  auto start = std::chrono::steady_clock::now();
  {
    Logger logger(path);
    std::vector<std::thread> workers;
    workers.reserve(kThreads);
    for (int t = 0; t < kThreads; ++t) {
      workers.emplace_back([t, &logger]() {
        for (int i = 0; i < kMessagesPerThread; ++i) {
          logger.log("T{} {}", t, i);
        }
      });
    }
    for (auto &w : workers) {
      w.join();
    }
  }
  auto elapsed = std::chrono::steady_clock::now() - start;
  std::cout << "[HighPressure] threads=" << kThreads
            << " messages_per_thread=" << kMessagesPerThread
            << " elapsed_ms="
            << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()
            << '\n';
  EXPECT_EQ(count_lines(path),
            static_cast<std::size_t>(kThreads * kMessagesPerThread));
  std::filesystem::remove(path);
}

TEST(LoggerStress, ThroughputIsAcceptable) {
  const auto path = unique_log_file("throughput");
  constexpr int kMessages = 100000;
  auto start = std::chrono::steady_clock::now();
  {
    Logger logger(path);
    for (int i = 0; i < kMessages; ++i) {
      logger.log("Throughput {}", i);
    }
  }
  auto elapsed = std::chrono::steady_clock::now() - start;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
  ASSERT_GT(ms, 0);
  auto messages_per_sec =
      static_cast<double>(kMessages) * 1000.0 / static_cast<double>(ms);
  std::cout << "[Throughput] messages=" << kMessages << " elapsed_ms=" << ms
            << " throughput_msg_per_sec=" << messages_per_sec << '\n';
  EXPECT_GT(messages_per_sec, 500.0);
  std::filesystem::remove(path);
}

TEST(LoggerBasic, SingleMessagePersisted) {
  const auto path = unique_log_file("single");
  {
    Logger logger(path);
    logger.log("Hello {}", "World");
  }
  std::ifstream in(path);
  std::string line;
  EXPECT_TRUE(std::getline(in, line));
  EXPECT_EQ(line, "Hello World");
  std::filesystem::remove(path);
}

TEST(LoggerBasic, FormatKeepsUnmatchedPlaceholders) {
  const auto path = unique_log_file("format");
  std::string formatted;
  {
    Logger logger(path);
    formatted = logger.format("Value {} {}", 42);
  }
  EXPECT_EQ(formatted, "Value 42 {}");
  std::filesystem::remove(path);
}