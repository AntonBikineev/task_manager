#ifndef BKNV_TASK_MANAGER_HPP
#define BKNV_TASK_MANAGER_HPP

#include <queue>
#include <memory>
#include <type_traits>

#include <mutex>
#include <condition_variable>

namespace bknv {

  inline namespace v1 {

    // task_manager class implementation
    template<
      class Task,
      class Compare = std::less<Task>
    >
    class task_manager
    {
    public:
      task_manager() = default;

      void push(const Task& task)
      {
        std::lock_guard<std::mutex> lock{mutex};
        queue.push(task);
        cv.notify_one();
      }

      Task pop() noexcept(std::is_nothrow_move_constructible<Task>::value)
      {
        std::unique_lock<std::mutex> lock{mutex};
        cv.wait(lock, [this]{ return !queue.empty(); });
        const auto ret = std::move(queue.top()); //nothrow
        queue.pop();
        return ret;
      }

      bool empty() const
      {
        return queue.empty();
      }

      size_t size() const
      {
        return queue.size();
      }

    private:
      std::priority_queue<Task, std::vector<Task>, Compare> queue;

      mutable std::mutex mutex;
      std::condition_variable cv;
    };

  }

}

#endif // BKNV_TASK_MANAGER_HPP
