#include "fscompsoc/async/event.hpp"

#include <vector>

using namespace std;

namespace fscompsoc::async
{
  template <typename T>
  shared_ptr<event_token<T>> event<T>::create_token()
  {
    shared_ptr<event_token<T>> ret(new event_handler<T>);

    ret->parent = this;

    tokens_mutex.lock();
    tokens.push_front(ret);
    ret->pos = tokens.front;
    tokens_mutex.unlock();

    return ret;
  }

  template <typename T>
  shared_ptr<event_handler<T>>
  event_token<T>::add_handler(std::function<T()> handler)
  {
    shared_ptr<event_token<T>> ret(new event_token<T>);

    ret->handler = handler;
    ret->parent  = this;

    handlers_mutex.lock();
    handlers.push_front(ret);
    ret->pos = handlers.front;
    handlers_mutex.unlock();

    return ret;
  }

  template <typename T>
  event_token<T>::~event_token()
  {
    // We need to lock to make sure that we are not accessed whilst being
    // deleted
    parent->tokens_mutex.lock();
    parent->tokens.erase(pos);
    parent->tokens_mutex.unlock();
  }

  template <typename T>
  event_handler<T>::~event_handler()
  {
    // We need to lock to make sure that we are not accessed whilst being
    // deleted
    parent->handlers_mutex.lock();
    parent->handlers.erase(pos);
    parent->handlers_mutex.unlock();
  }

  template <typename T>
  void event<T>::trigger(T t)
  {
    vector<thread> threads;

    tokens_mutex.lock();
    threads.reserve(tokens.size());

    for (auto& token : tokens)
      threads.push_back(token.trigger(t));

    for (auto& thread : threads)
      thread.join();

    tokens_mutex.unlock();
  }

  template <typename T>
  thread event_token<T>::trigger(T t)
  {
    return thread([this, t] {
      handlers_mutex.lock();

      for (auto& handler : handlers)
        handler(t);

      handlers_mutex.unlock();
    });
  }

} // namespace fscompsoc::async
