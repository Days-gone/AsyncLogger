#include <gtest/gtest.h>
#include "SafeQueue.hpp"
#include <thread>
#include <vector>
#include <chrono>

TEST(SafeQueueTest, EnqueueDequeueWorks) {
    SafeQueue<int> q(10);
    EXPECT_TRUE(q.empty());

    q.enqueue(42);
    EXPECT_FALSE(q.empty());

    int val = q.dequeue();
    EXPECT_EQ(val, 42);
    EXPECT_TRUE(q.empty());
}

TEST(SafeQueueTest, MultipleEnqueueDequeue) {
    SafeQueue<int> q(5);
    for (int i = 0; i < 5; ++i) {
        q.enqueue(i);
    }
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(q.dequeue(), i);
    }
    EXPECT_TRUE(q.empty());
}

// 多线程并发测试
TEST(SafeQueueTest, MultiThreadedEnqueueDequeue) {
    SafeQueue<int> q(1000);
    const int num_threads = 4;
    const int num_per_thread = 250;

    // 生产者线程
    std::vector<std::thread> producers;
    for (int t = 0; t < num_threads; ++t) {
        producers.emplace_back([&q, t, num_per_thread]() {
            for (int i = 0; i < num_per_thread; ++i) {
                q.enqueue(t * num_per_thread + i);
            }
        });
    }

    // 消费者线程
    std::vector<int> results(1000);
    std::vector<std::thread> consumers;
    for (int t = 0; t < num_threads; ++t) {
        consumers.emplace_back([&q, &results, t, num_per_thread]() {
            for (int i = 0; i < num_per_thread; ++i) {
                int val = q.dequeue();
                results[val] = 1;
            }
        });
    }

    for (auto& th : producers) th.join();
    for (auto& th : consumers) th.join();

    int sum = 0;
    for (int v : results) sum += v;
    EXPECT_EQ(sum, 1000);
}

// 性能测试
TEST(SafeQueueTest, PerformanceEnqueueDequeue) {
    SafeQueue<int> q(100000);
    const int N = 100000;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) {
        q.enqueue(i);
    }
    for (int i = 0; i < N; ++i) {
        q.dequeue();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // 性能阈值可根据实际机器调整
    EXPECT_LT(duration, 500); // 500ms 内完成
}

TEST(SafeQueueTest, LargeScaleEnqueueDequeue) {
    const int N = 1000000; // 一百万数据
    SafeQueue<int> q(N);

    // 批量入队
    for (int i = 0; i < N; ++i) {
        q.enqueue(i);
    }
    EXPECT_FALSE(q.empty());

    // 批量出队并校验顺序
    for (int i = 0; i < N; ++i) {
        EXPECT_EQ(q.dequeue(), i);
    }
    EXPECT_TRUE(q.empty());
}

TEST(SafeQueueTest, MultiThreadedLargeScalePerformance) {
    const int N = 1000000; // 一百万数据
    const int num_threads = 8;
    SafeQueue<int> q(N);

    // 生产者线程
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> producers;
    for (int t = 0; t < num_threads; ++t) {
        producers.emplace_back([&q, t, N, num_threads]() {
            int chunk = N / num_threads;
            int begin = t * chunk;
            int end = (t == num_threads - 1) ? N : begin + chunk;
            for (int i = begin; i < end; ++i) {
                q.enqueue(i);
            }
        });
    }
    for (auto& th : producers) th.join();

    EXPECT_FALSE(q.empty());

    // 消费者线程
    std::vector<int> results(N, 0);
    std::vector<std::thread> consumers;
    for (int t = 0; t < num_threads; ++t) {
        consumers.emplace_back([&q, &results, t, N, num_threads]() {
            int chunk = N / num_threads;
            for (int i = 0; i < chunk; ++i) {
                int val = q.dequeue();
                results[val] = 1;
            }
        });
    }
    for (auto& th : consumers) th.join();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    int sum = 0;
    for (int v : results) sum += v;
    EXPECT_EQ(sum, N);
    EXPECT_TRUE(q.empty());

    // 性能阈值可根据实际机器调整
    EXPECT_LT(duration, 2000); // 2秒内完成
}