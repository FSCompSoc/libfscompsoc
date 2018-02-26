#pragma once

#include "fscompsoc/exceptions.hpp"

#include <chrono>
#include <functional>
#include <future>
#include <optional>
#include <thread>

namespace fscompsoc::async
{
  // NOTE: we have to implement this seperately from action,
  // as optional<void> is invalid
  class attempt
  {
  private:
    std::thread _t;
    std::optional<std::function<void()>> on_cancel;
    bool running = true;
    std::future<bool> ret;

  public:
    inline bool is_running() { return running; }

    inline bool is_cancellable() { return on_cancel.has_value(); }
    inline void cancel()
    {
      if (!running)
        return;
      if (on_cancel) {
        (*on_cancel)();
        running = false;
      } else
        throw exceptions::InvalidOperation();
    }

    inline bool try_cancel()
    {
      if (!running)
        return false;
      if (on_cancel) {
        (*on_cancel)();
        running = false;
        return true;
      } else
        return false;
    }

    inline bool result()
    {
      if (!running)
        throw exceptions::InvalidOperation();

      return ret.get();
    }

    template <typename Rep, typename Period>
    inline bool result(const std::chrono::duration<Rep, Period>& rel_time)
    {
      if (!running)
        throw exceptions::InvalidOperation();

      if (ret.wait_for(rel_time) == std::future_status::ready)
        return ret.get();
      else
        throw exceptions::TimedOut();
    }

    template <typename Rep, typename Period>
    inline bool result(const std::chrono::time_point<Rep, Period>& abs_time)
    {
      if (!running)
        throw exceptions::InvalidOperation();

      if (ret.wait_until(abs_time) == std::future_status::ready)
        return ret.get();
      else
        throw exceptions::TimedOut();
    }

  public:
    attempt(std::function<bool()> func);
    attempt(std::function<bool()> func, std::function<void()> cancel);

    ~attempt();
  };
} // namespace fscompsoc::async
