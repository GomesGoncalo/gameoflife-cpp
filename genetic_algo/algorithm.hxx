#pragma once

#include <asio/io_context.hpp>

namespace algo {
struct serialized {};

struct parallel {
  asio::io_context &ctx;
};

template <typename Container, typename Callable>
void for_each(parallel &&p, Container &&container, Callable &&callable);

template <typename Container, typename Callable>
void for_each(serialized &&, Container &&container, Callable &&callable);
} // namespace algo

#include "detail/algorithm.hxx"
