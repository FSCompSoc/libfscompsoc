#include "fscompsoc/async/event.hpp"

namespace fscompsoc::async
{
  template <typename T>
  event_token<T> event<T>::create_token()
  {
    event_token<T> ret;

    tokens_mutex.lock();
    ret.push_front();
    tokens_mutex.unlock();

    return ret;
  }
} // namespace fscompsoc::async
