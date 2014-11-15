#include <iostream>
#include <algorithm>
#include <functional>

#include <thread>
#include <future>
#include <array>
#include <atomic>

#include "task_manager.hpp"
#include "task.hpp"

int main()
{
  using namespace bknv;
  task_manager<task> a;

  std::thread{[&]
  {
    std::array<std::thread, 100u> threads;

    size_t i = -1;
    std::generate(std::begin(threads), std::end(threads), [&]
    {
      ++i;
      return std::thread{[&a, i]
      {
          a.push(task{[i]
          {
            std::cout << "task with priority: " << i << std::endl;
          },
          i});
      }};
    });

    std::for_each(std::begin(threads), std::end(threads), [&](std::thread& thr)
    {
      thr.join();
    });
  }}.join();

  std::thread{[&]
  {
    std::array<std::thread, 100u> threads;

    std::generate(std::begin(threads), std::end(threads), [&]
    {
      return std::thread([&]
      {
        auto task = a.pop();
        static std::mutex mutex;
        std::lock_guard<std::mutex>lock{mutex};
        task();
      });
    });

    std::for_each(std::begin(threads), std::end(threads), [&](std::thread& thr)
    {
      thr.join();
    });
  }}.join();
}
