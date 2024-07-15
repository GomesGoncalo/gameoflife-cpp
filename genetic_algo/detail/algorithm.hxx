#pragma once

#include <asio/post.hpp>

#include <algorithm>
#include <condition_variable>
#include <mutex>

namespace algo {
template <typename Container, typename Callable>
void for_each(parallel &&p, Container &&container, Callable &&callable) {
  const auto number_of_splits = p.threads;
  auto counter = number_of_splits;
  std::mutex mtx;
  std::condition_variable cv;
  auto notify_till_done = [&counter, &mtx, &cv](const bool wait = false) {
    std::unique_lock<std::mutex> lk{mtx};
    --counter;
    if (counter == 0) {
      cv.notify_all();
    }
    if (wait && counter) {
      cv.wait(lk, [&counter] { return counter == 0; });
    }
  };

  auto begin = container.begin();
  auto end = container.end();

  const auto split_size = std::distance(begin, end) / number_of_splits;

  auto cursor = begin;
  for (auto split = 1u; split < number_of_splits; ++split) {
    auto split_begin = cursor;
    auto split_end = cursor;
    std::advance(split_end, split_size);
    cursor = split_end;
    asio::post(p.ctx, [split_begin, split_end, &callable, &notify_till_done] {
      std::for_each(split_begin, split_end, callable);
      notify_till_done();
    });
  }

  std::for_each(cursor, end, callable);
  notify_till_done(true);
}

template <typename Container, typename Callable>
void for_each(serialized &&, Container &&container, Callable &&callable) {
  std::for_each(container.begin(), container.end(), callable);
}
} // namespace algo
