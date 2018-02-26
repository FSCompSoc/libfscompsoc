#pragma once

#include <functional>
#include <list>
#include <mutex>
#include <thread>

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
    typename std::list<std::shared_ptr<event_handler<T>>>::iterator pos;
    event_token<T>* parent;

  public:
    inline void operator()(T t) { handler(t); }

  public:
    event_handler(event_handler&) = delete;

    ~event_handler();
  };

  template <typename T>
  class event_token
  {
    friend event<T>;
    friend event_handler<T>;

  private:
    std::list<std::shared_ptr<event_handler<T>>> handlers;
    std::mutex handlers_mutex;
    typename std::list<std::shared_ptr<event_token<T>>>::iterator pos;
    event<T>* parent;

  public:
    std::shared_ptr<event_handler<T>> add_handler(std::function<T()> handler);

  private:
    std::thread trigger(T);

  private:
    event_token() = default;

  public:
    event_token(event_token&);

    ~event_token();
  };

  template <typename T>
  class event
  {
    friend event_token<T>;

  private:
    std::list<std::shared_ptr<event_token<T>>> tokens;
    std::mutex tokens_mutex;

  public:
    std::shared_ptr<event_token<T>> create_token();

    void trigger(T);

    inline void operator()(T t) { trigger(t); }
  };
} // namespace fscompsoc::async
