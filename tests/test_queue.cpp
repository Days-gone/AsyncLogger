#include <gtest/gtest.h>
#include "SafeQueue.hpp"
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

TEST(SafeQueueTest, MultiThreadedLargeScaleCorrectnessAndDeadlock) {
    const int N = 1000000;
    const int num_threads = 8;
    SafeQueue<int> q(N);

    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};
    std::vector<int> results(N, 0);

    // 生产者线程
    std::vector<std::thread> producers;
    for (int t = 0; t < num_threads; ++t) {
        producers.emplace_back([&q, &produced, t, N, num_threads]() {
            int chunk = N / num_threads;
            int begin = t * chunk;
            int end = (t == num_threads - 1) ? N : begin + chunk;
            for (int i = begin; i < end; ++i) {
                q.enqueue(i);
                ++produced;
            }
        });
    }

    // 消费者线程
    std::vector<std::thread> consumers;
    for (int t = 0; t < num_threads; ++t) {
        consumers.emplace_back([&q, &results, &consumed, N, num_threads]() {
            int chunk = N / num_threads;
            for (int i = 0; i < chunk; ++i) {
                int val = -1;
                bool ok = q.dequeue(val);
                if (ok) {
                    results[val]++;
                    ++consumed;
                }
            }
        });
    }

    // 死锁检测：设置最大等待时间
    auto start = std::chrono::high_resolution_clock::now();
    for (auto& th : producers) th.join();
    for (auto& th : consumers) th.join();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

    // 检查所有数据都被消费且无重复
    int total = 0;
    for (int v : results) total += v;
    EXPECT_EQ(total, N);
    for (int v : results) EXPECT_EQ(v, 1);

    // 检查生产和消费数量
    EXPECT_EQ(produced.load(), N);
    EXPECT_EQ(consumed.load(), N);

    // 死锁检测（测试时间不应过长）
    EXPECT_LT(duration, 10); // 10秒内完成
}