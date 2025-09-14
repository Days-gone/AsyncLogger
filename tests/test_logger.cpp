#include <Logger.hpp>
#include <iostream>

int main() {
    Logger logger("test.log");

    logger.log("Hello, {}!", "world");
    logger.log("The answer is: {}", 42);
    logger.log("Multiple values: {}, {}, {}", "A", 123, 4.56);

    // 测试 format 方法
    std::string msg = logger.format("Test format: {}, {}", "foo", "bar");
    std::cout << msg << std::endl;
    logger.shutdown();

    return 0;
}