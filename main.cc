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
  task_manager<safe_task> tm;
  std::mutex mutex;

  std::thread{[&]
  {
    std::array<std::thread, 100u> threads;

    size_t i = -1;
    std::generate(std::begin(threads), std::end(threads), [&]
    {
      ++i;
      return std::thread{[&tm, &mutex, i]
      {
          tm.push(safe_task{mutex, [i]
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
      return std::thread{[&]
      {
        tm.pop()();
      }};
    });

    std::for_each(std::begin(threads), std::end(threads), [&](std::thread& thr)
    {
      thr.join();
    });
  }}.join();

}
