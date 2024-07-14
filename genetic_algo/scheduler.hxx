#pragma once

#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <thread>
#include <vector>

struct scheduler {
  scheduler(asio::io_context &ctx,
            const unsigned int num = std::thread::hardware_concurrency() - 1);
  ~scheduler();

private:
  asio::io_context &ctx;
  asio::executor_work_guard<
      decltype(std::declval<asio::io_context &>().get_executor())>
      work;
  std::vector<std::jthread> threads;
};
