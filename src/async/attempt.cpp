#include "fscompsoc/async/attempt.hpp"

using namespace std;

namespace fscompsoc::async {
  attempt::attempt(std::function<bool()> func) {
    promise<bool> result;
    ret = result.get_future();

    _t = thread([func, result = move(result)]() mutable {
      bool v = func();
      result.set_value(v);
    });
  }

  attempt::attempt(std::function<bool()> func, std::function<void()> cancel) :
    on_cancel(cancel)
  {
    on_cancel = cancel;
    promise<bool> result;
    ret = result.get_future();

    _t = thread([func, result = move(result)]() mutable {
      bool v = func();
      result.set_value(v);
    });
  }

  attempt::~attempt() {
    try_cancel();
    _t.detach();
  }
}
