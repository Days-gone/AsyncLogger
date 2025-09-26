# AsyncLogger
本仓库用于 C++ 实现异步日志器。

## 环境搭建
本项目使用 `cmake` 与 `g++`，依赖通过 `vcpkg` 管理。

```bash
sudo apt install g++ cmake
export VCPKG_ROOT="/path/to/your/vcpkg"
./vcpkg install gtest
```

## 构建与测试
```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build build
ctest --test-dir build
```

## 实现步骤概述

### 1. 实现 SafeQueue
构建线程安全队列，提供 `enqueue`、`dequeue`、`shutdown` 等接口，利用 `std::mutex` 与 `std::condition_variable` 保证在高并发下的安全访问，并在关闭后阻止新元素加入、唤醒等待线程。

### 2. 实现 Logger
- 构造时打开日志文件并启动后台线程。
- `log()` 负责格式化字符串并入队。
- `process()` 在独立线程中循环 `dequeue`，将日志写入文件。
- 析构或显式调用 `shutdown()` 时停止队列并等待线程退出，确保剩余日志全部落盘。

### 3. 支持简单格式化
使用 `{}` 作为占位符，逐个替换可变参数；多出来的占位符保持原样，便于诊断。

## 示例
```cpp
#include <Logger.hpp>

int main() {
  Logger logger("app.log");
  logger.log("User {} logged in", 42);
  return 0;
}
```

## 目录结构
- `include/`：`SafeQueue` 与 `Logger` 接口
- `tests/`：基于 Google Test 的功能与压力测试
- `CMakeLists.txt`：构建配置

## 运行测试
核心测试覆盖：
- 析构时无死锁 (`LoggerStress.NoDeadlockOnDestruction`)
- 多线程高压写入 (`LoggerStress.MultiThreadedHighPressure`)
- 吞吐量评估 (`LoggerStress.ThroughputIsAcceptable`)
- 基础功能 (`LoggerBasic.*`)

执行 `ctest` 后可在 `build/Testing/Temporary` 查看测试日志。