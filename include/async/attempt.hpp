#pragma once

#include <future>
#include <optional>

namespace fscompsoc::async {
  using attempt = std::future<bool>;

  template<typename T>
  using maybe = std::future<std::optional<T>>;
}
