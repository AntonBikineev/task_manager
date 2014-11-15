#ifndef BKNV_TASK_HPP
#define BKNV_TASK_HPP

#include <memory>

namespace bknv {

  inline namespace v1 {

    class task
    {
    public:
      template <class Function>
      task(const Function& f, size_t priority):
        p{priority},
        function(std::make_unique<derived_erased<Function>>(f))
      {
      }

      task(const task& other):
        p{other.p},
        function{other.function->make_copy()}
      {
      }

      task(task&&) = default;

      task& operator=(const task& other)
      {
        if (this == &other)
          return *this;

        p = other.p;
        function.reset(other.function->make_copy());

        return *this;
      }

      task& operator=(task&& other) = default;

      size_t priority() const
      {
        return p;
      }

      void operator()()
      {
        function->call();
      }

    private:
      struct base_erased
      {
        virtual ~base_erased() = default;
        virtual void call() = 0;
        virtual base_erased* make_copy() = 0;
      };

      template <class Function>
      struct derived_erased: base_erased
      {
        derived_erased(const Function& function):
          function(function)
        {
        }

        virtual void call() override
        {
          function();
        }

        virtual derived_erased* make_copy() override
        {
          return new derived_erased<Function>{function};
        }

        Function function;
      };

      size_t p;
      std::unique_ptr<base_erased> function;
    };

    bool operator<(const task& a, const task& b)
    {
      return a.priority() < b.priority();
    }

  }

}

#endif // BKNV_TASK_HPP
