#pragma once

#include <asio/post.hpp>

#include <algorithm>
#include <condition_variable>
#include <mutex>

namespace algo {
template <typename Container, typename Callable>
void for_each(parallel &&p, Container &&container, Callable &&callable) {
  const auto number_of_splits = p.threads;
  std::atomic<bool> running{true};
  struct holder {
    ~holder() noexcept { fn(); }
    std::function<void()> fn;
  };

  auto holders = std::make_shared<holder>([&running] noexcept {
    running.store(false, std::memory_order::seq_cst);
    running.notify_all();
  });
  std::weak_ptr<holder> wp{holders};
  auto begin = container.begin();
  auto end = container.end();

  const auto split_size = std::distance(begin, end) / number_of_splits;

  auto cursor = begin;
  for (auto split = 1u; split < number_of_splits; ++split) {
    auto split_begin = cursor;
    auto split_end = cursor;
    std::advance(split_end, split_size);
    cursor = split_end;
    asio::post(p.ctx, [split_begin, split_end, &callable, holders] {
      std::for_each(split_begin, split_end, callable);
    });
  }
  holders.reset();
  std::for_each(cursor, end, callable);
  running.wait(true, std::memory_order::seq_cst);
}

template <typename Container, typename Callable>
void for_each(serialized &&, Container &&container, Callable &&callable) {
  std::for_each(container.begin(), container.end(), callable);
}
} // namespace algo
