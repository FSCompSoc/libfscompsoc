#pragma once

#include "fscompsoc/exceptions.hpp"

#include <future>
#include <optional>
#include <thread>
#include <functional>
#include <chrono>

namespace fscompsoc::async {
  template<typename T>
  class action {
  private:
    std::thread _t;
    std::optional<std::function<void()>> on_cancel;
    bool running = true;
    std::future<std::optional<T>> ret;

  public:
    inline bool is_running() { return running; }

    inline bool is_cancellable() { return on_cancel.has_value(); }
    inline void cancel() {
      if(on_cancel) {
        (*on_cancel)();
        running = false;
      }
      else
        throw exceptions::InvalidOperation();
    }
    inline bool try_cancel() {
      if(on_cancel) {
        (*on_cancel)();
        running = false;
        return true;
      } else
        return false;
    }

    inline std::optional<T> get() {
      if (!running)
        throw exceptions::InvalidOperation();

      return ret.get();
    }

    template <typename Rep, typename Period>
    inline std::optional<T> get(
      const std::chrono::duration<Rep,Period>& rel_time
    ) {
      if (!running)
        throw exceptions::InvalidOperation();

      if (ret.wait_for(rel_time) == std::future_status::ready)
        return ret.get();
      else
        throw exceptions::TimedOut();
    }

    template <typename Rep, typename Period>
    inline std::optional<T> get(
      const std::chrono::time_point<Rep,Period>& abs_time
    ) {
      if (!running)
        throw exceptions::InvalidOperation();

      if (ret.wait_until(abs_time) == std::future_status::ready)
        return ret.get();
      else
        throw exceptions::TimedOut();
    }

  public:
    action(std::function<T()> func);
    action(std::function<std::optional<T>()> func);

    action(std::function<T()> func, std::function<void()> cancel);
    action(std::function<std::optional<T>()> func, std::function<void()> cancel);
  };
}

#include "fscompsoc/async/action.tpp"
