#pragma once

#include <functional>
#include <list>
#include <mutex>

namespace fscompsoc::async
{
  template <typename T>
  class event;

  template <typename T>
  class event_token;

  template <typename T>
  class event_handler
  {
    friend event_token<T>;

  private:
    std::function<void(T)> handler;
    typename std::list<T>::iterator pos;

  public:
    void disconnect();
  };

  template <typename T>
  class event_token
  {
    friend event<T>;

  private:
    std::list<event_handler<T>> handlers;
    std::mutex handlers_mutex;

  public:
    event_handler<T> add_handler(std::function<T()> handler);

  private:
    void trigger(T);

  private:
    event_token() = default;
  };

  template <typename T>
  class event
  {
  private:
    std::list<event_token<T>> tokens;
    std::mutex tokens_mutex;

  public:
    event_token<T> create_token();

    inline void trigger(T);
  };
} // namespace fscompsoc::async
