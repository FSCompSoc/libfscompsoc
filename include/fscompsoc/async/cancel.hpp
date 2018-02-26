#pragma once

#include <memory>

namespace fscompsoc::async
{
  class canceller;

  class cancel_token
  {
    friend canceller;

  private:
    const std::shared_ptr<bool> token;

  public:
    inline bool is_cancelled() { return *token; }

  private:
    inline cancel_token(std::shared_ptr<bool> token) : token(token) {}
  };

  class canceller
  {
  private:
    std::shared_ptr<bool> token;

  public:
    inline void cancel() { *token = true; }
    inline cancel_token get_token() { return cancel_token(token); }

  public:
    inline canceller() : token(new bool(false)) {}
  };
} // namespace fscompsoc::async
