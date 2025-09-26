#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <queue>

template <typename T> class SafeQueue {
public:
  SafeQueue(size_t max_len);
  ~SafeQueue();

  /* Function */
  auto enqueue(T ele) -> void;
  auto dequeue(T &ref) -> bool;
  auto shutdown() -> void;

private:
  size_t max_len_ = 10;
  std::mutex mtx_;
  std::condition_variable cv_;
  std::queue<T> queue_;
  bool shut_down_ = false;
};

/**
 * @brief Construct a new Safe Queue< T>:: Safe Queue object
 *
 * @tparam T
 * @param max_len
 */
template <typename T>
inline SafeQueue<T>::SafeQueue(size_t max_len) : max_len_(max_len) {}
/**
 * @brief Destroy the Safe Queue< T>:: Safe Queue object
 *
 * @tparam T
 */
template <typename T> inline SafeQueue<T>::~SafeQueue() {}

/**
 * @brief Enqueue a element into the queue, if the queue has been full, then do
 * nothing.
 *
 * @tparam T
 * @param ele
 */
template <typename T> inline auto SafeQueue<T>::enqueue(T ele) -> void {
  std::unique_lock<std::mutex> lock(this->mtx_);
  if (this->shut_down_) {
    return ;
  }

  while (this->queue_.size() >= this->max_len_ && !this->shut_down_) {
    this->cv_.wait(lock);
  }
  if (this->shut_down_) {
    return ;
  }
  this->queue_.push(std::move(ele));
  this->cv_.notify_one();
}

/**
 * @brief Return a value from the queue. Should always call empty() before
 * calling this function.
 *
 * @tparam T
 * @return T
 */
template <typename T> inline auto SafeQueue<T>::dequeue(T &ref) -> bool {
  std::unique_lock<std::mutex> lock(this->mtx_);

  while (this->queue_.empty() && !this->shut_down_) {
    this->cv_.wait(lock);
  }
  if (this->queue_.empty()) {
    return false;
  }
  
  ref = std::move(this->queue_.front());
  this->queue_.pop();
  this->cv_.notify_one();
  return true;
}
/**
 * @brief Shut down the queue.
 *
 * @tparam T
 */
template <typename T> inline auto SafeQueue<T>::shutdown() -> void {
  std::unique_lock<std::mutex> lock(this->mtx_);
  this->shut_down_ = true;
  this->cv_.notify_all();
}