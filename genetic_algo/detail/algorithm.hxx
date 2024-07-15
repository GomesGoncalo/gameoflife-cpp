#pragma once

#include <asio/post.hpp>

#include <algorithm>
#include <condition_variable>
#include <mutex>

namespace algo {
template <typename Container, typename Callable>
void for_each(parallel &&p, Container &&container, Callable &&callable) {
  std::mutex mtx;
  std::condition_variable cv;
  const auto number_of_splits = p.threads;

  auto begin = container.begin();
  auto end = container.end();

  const auto split_size = std::distance(begin, end) / number_of_splits;

  auto counter = number_of_splits;
  auto cursor = begin;
  for (auto split = 1u; split < number_of_splits; ++split) {
    auto split_begin = cursor;
    auto split_end = cursor;
    std::advance(split_end, split_size);

    asio::post(p.ctx, [split_begin, split_end, &callable, &mtx, &cv,
                       &number_of_splits, &counter] {
      std::for_each(split_begin, split_end, callable);
      std::unique_lock<std::mutex> lk{mtx};
      --counter;
      if (counter == 0) {
        cv.notify_one();
      }
    });

    cursor = split_end;
  }

  std::for_each(cursor, end, callable);

  std::unique_lock<std::mutex> lk{mtx};
  --counter;
  if (counter == 0) {
    cv.notify_one();
  }
  cv.wait(lk, [&counter] { return counter == 0; });
}

template <typename Container, typename Callable>
void for_each(serialized &&, Container &&container, Callable &&callable) {
  std::for_each(container.begin(), container.end(), callable);
}
} // namespace algo
