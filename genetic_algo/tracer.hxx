#pragma once

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <list>
#include <mutex>

struct guard {
  friend struct tracer;

  ~guard() noexcept {
    std::lock_guard<std::mutex> lg{mutex};
    *it = std::make_pair(s, std::chrono::steady_clock::now() - started);
  }

private:
  template <typename StringLike>
  guard(std::mutex &mutex,
        std::list<std::optional<std::pair<
            std::string, std::chrono::duration<double>>>>::reverse_iterator it,
        StringLike &&s)
      : it{it}, started{std::chrono::steady_clock::now()}, mutex{mutex},
        s{std::forward<StringLike>(s)} {}
  std::list<std::optional<
      std::pair<std::string, std::chrono::duration<double>>>>::reverse_iterator
      it;
  std::chrono::time_point<std::chrono::steady_clock> started;
  std::mutex &mutex;
  std::string s;
};

struct tracer {
  static tracer &instance() {
    static tracer t;
    return t;
  }

  tracer(tracer &&) = delete;
  tracer(const tracer &) = delete;
  tracer &operator=(tracer &&) = delete;
  tracer &operator=(const tracer &) = delete;
  ~tracer() noexcept = default;

  template <typename StringLike> guard acquire(StringLike &&s) {
    std::lock_guard<std::mutex> lg{mutex};
    tracked.push_back(std::nullopt);
    return guard{mutex, tracked.rbegin(), std::forward<StringLike>(s)};
  }

  void report() const {
    struct stats {
      std::string name;
      std::vector<std::chrono::duration<double>> list;

      ~stats() noexcept {
        if (list.empty())
          return;
        const unsigned int count = list.size();
        std::chrono::duration<double> avg{0};
        std::chrono::duration<double> stddev{0};
        std::chrono::duration<double> min{std::numeric_limits<double>::max()};
        std::chrono::duration<double> max{0};
        for (const auto &l : list) {
          min = std::min(min, l);
          max = std::max(max, l);
          avg += l;
        }
        avg /= count;
        if (list.size() != 1) {
          for (const auto &l : list) {
            auto tmp = l - avg;
            stddev += std::chrono::duration<double>{std::pow(tmp.count(), 2)};
          }
          stddev /= count - 1;
          stddev = std::chrono::duration<double>{std::sqrt(stddev.count())};
        }
        std::cerr << name << "\n\tavg: " << avg << "\n\tmin: " << min
                  << "\n\tmax: " << max << "\n\tstddev: " << stddev << "\n";
      }
    };
    std::list<
        std::optional<std::pair<std::string, std::chrono::duration<double>>>>
        ltracked;
    {
      std::lock_guard<std::mutex> lg{mutex};
      ltracked = tracked;
    }
    std::unordered_map<std::string, stats> stats_map;
    for (const auto &t : ltracked) {
      if (!t)
        continue;
      auto [it, _] = stats_map.emplace(
          std::piecewise_construct, std::forward_as_tuple(t->first),
          std::forward_as_tuple(t->first,
                                std::vector<std::chrono::duration<double>>{}));
      it->second.list.push_back(t->second);
    }
  }

  std::shared_ptr<asio::steady_timer>
  report_every(asio::io_context &ctx,
               std::chrono::milliseconds periodicity) const {
    auto timer = std::make_shared<asio::steady_timer>(ctx);
    report_every(timer, std::move(periodicity));
    return timer;
  }

private:
  tracer() = default;

  void report_every(std::weak_ptr<asio::steady_timer> timer,
                    std::chrono::milliseconds periodicity) const {
    if (auto stimer = timer.lock()) {

      stimer->expires_from_now(periodicity);
      stimer->async_wait(
          [this, timer = std::move(timer),
           periodicity = std::move(periodicity)](const auto &ec) {
            if (!!ec) {
              return;
            }

            report();
            report_every(timer, periodicity);
          });
    }
  }

  mutable std::mutex mutex;
  std::list<
      std::optional<std::pair<std::string, std::chrono::duration<double>>>>
      tracked;
};
