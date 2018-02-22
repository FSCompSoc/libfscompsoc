#pragma once

#include <future>
#include <optional>
#include <thread>
#include <functional>

namespace fscompsoc::async {
  class attempt {
  private:
    std::thread _t;
    std::function on_cancel<void()>;
    std::future<bool> ret;

  public:
    inline void cancel() { on_cancel(); }
    inline bool wait() { ret.wait(); }

  public:
    attempt()
  }

  using attempt = std::future<bool>;

  template<typename T>
  using maybe = std::future<std::optional<T>>;
}
