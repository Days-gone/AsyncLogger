#pragma once

#include <cstddef>
#include <mutex>
#include <queue>

template <typename T> class SafeQueue {
public:
  SafeQueue(size_t max_len);
  ~SafeQueue();

  /* Function */
  auto enqueue(T ele) -> void;
  auto dequeue() -> T;
  auto empty() -> bool;

private:
  size_t max_len_;
  std::mutex mtx_;
  std::queue<T> queue;
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
  this->mtx_.lock();
  if (this->queue.size() >= this->max_len_) {
    this->mtx_.unlock();
    return;
  }
  this->queue.push(ele);
  this->mtx_.unlock();
}

/**
 * @brief Return a value from the queue. Should always call empty() before
 * calling this function.
 *
 * @tparam T
 * @return T
 */
template <typename T> inline auto SafeQueue<T>::dequeue() -> T {
  this->mtx_.lock();
  T ele = this->queue.front();
  this->queue.pop();
  this->mtx_.unlock();
  return ele;
}

/**
 * @brief Check whether the Queue is empty or not, return a bool value.
 *
 * @tparam T
 * @return true
 * @return false
 */
template <typename T> inline auto SafeQueue<T>::empty() -> bool {
  this->mtx_.lock();
  bool res = this->queue.empty();
  this->mtx_.unlock();
  return res;
}