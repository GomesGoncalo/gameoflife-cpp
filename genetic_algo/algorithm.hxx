#pragma once

#include <asio/io_context.hpp>
#include <thread>

namespace algo {
struct serialized {};

struct parallel {
  asio::io_context &ctx;
  unsigned int threads = std::thread::hardware_concurrency() - 1;
};

template <typename Container, typename Callable>
void for_each(parallel &&p, Container &&container, Callable &&callable);

template <typename Container, typename Callable>
void for_each(serialized &&, Container &&container, Callable &&callable);
} // namespace algo

#include "detail/algorithm.hxx"
