#pragma once

#include <asio/io_context.hpp>
#include <thread>
#include <vector>

struct scheduler : public asio::io_context {
  scheduler(asio::io_context &main_ctx);
  ~scheduler();
  scheduler(scheduler &&) = delete;
  scheduler(const scheduler &) = delete;
  scheduler &operator=(scheduler &&) = delete;
  scheduler &operator=(const scheduler &) = delete;

  void stop() {
    asio::io_context::stop();
    main_ctx.stop();
  }

  asio::io_context &main_thread() noexcept { return main_ctx; }

private:
  asio::io_context &main_ctx;
  std::vector<std::thread> threads;
};
