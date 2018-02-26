#pragma once

#define __FSCOMPSOC_ACTION_CONSTRUCTOR                                         \
  std::promise<std::optional<T>> result;                                       \
  ret = result.get_future();                                                   \
  _t  = std::thread([func, result = std::move(result)]() mutable {             \
    std::optional<T> v = func();                                              \
    result.set_value(v);                                                      \
  });

namespace fscompsoc::async
{
  template <typename T>
  action<T>::action(std::function<T()> func)
  {
    __FSCOMPSOC_ACTION_CONSTRUCTOR
  }

  template <typename T>
  action<T>::action(std::function<std::optional<T>()> func)
  {
    __FSCOMPSOC_ACTION_CONSTRUCTOR
  }

  template <typename T>
  action<T>::action(std::function<T()> func, std::function<void()> cancel)
      : on_cancel(cancel)
  {
    __FSCOMPSOC_ACTION_CONSTRUCTOR
  }

  template <typename T>
  action<T>::action(std::function<std::optional<T>()> func,
                    std::function<void()> cancel)
      : on_cancel(cancel)
  {
    __FSCOMPSOC_ACTION_CONSTRUCTOR
  }
} // namespace fscompsoc::async

#undef __FSCOMPSOC_ACTION_CONSTRUCTOR
