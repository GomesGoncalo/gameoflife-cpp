#pragma once

#include <asio/io_context.hpp>
#include <thread>
#include <vector>

struct scheduler : public asio::io_context {
  unsigned int size() const noexcept { return size_; }

  void stop() { main_ctx.stop(); }

  asio::io_context &main_thread() noexcept { return main_ctx; }

  void run();

private:
  asio::io_context main_ctx;
  unsigned int size_ = std::thread::hardware_concurrency();
};
