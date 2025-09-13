# AsyncLogger
本仓库用于 C++ 实现异步日志器。

## 步骤

**1.环境搭建**

本项目使用 `cmake` 和 `g++`，并通过 `vcpkg` 管理依赖包。  
请按如下步骤配置环境：
```bash
sudo apt install g++ cmake
export VCPKG_ROOT="/path/to/your/vcpkg"
```

**2.实现 SafeQueue**

我们需要一个线程安全的队列，首先实现这个多线程安全的队列。  
在不考虑队列在生命周期中shut_down的情况下，先实现`enqueue`和`dequeue`两个方法。  
队列首先有一个最大容量，其次还需要使用`mutex`和`condition_variable`来实现高效的线程间协同。
```cpp
template <typename T> class SafeQueue {
public:
  SafeQueue(size_t max_len);
  ~SafeQueue();

  /* Function */
  auto enqueue(T ele) -> void;
  auto dequeue() -> T;
  auto empty() -> bool;

private:
  size_t max_len_ = 0;
  std::mutex mtx_;
  std::condition_variable cv_;
  std::queue<T> queue;
  bool shut_down_ = false;
};
```
接下来实现一个不考虑shu_down的方法集合,此处先不考虑构造函数和析构函数相关的函数实现。
```cpp
template <typename T> inline auto SafeQueue<T>::enqueue(T ele) -> void {
  std::unique_lock<std::mutex> lock(this->mtx_);

  while (this->queue_.size() >= this->max_len_) {
    this->cv_.wait(lock);
  }
  this->queue_.push(ele);
  this->cv_.notify_one();
}

template <typename T> inline auto SafeQueue<T>::dequeue(T &ref) -> bool {
  std::unique_lock<std::mutex> lock(this->mtx_);

  while (this->queue_.empty() && !this->shut_down_) {
    this->cv_.wait(lock);
  }
  if (this->shut_down_) {
    return false;
  }
  
  ref = this->queue_.front();
  this->queue_.pop();
  this->cv_.notify_one();
  return true;
}
```