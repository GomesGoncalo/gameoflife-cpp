#pragma once

#include <mutex>
#include <shared_mutex>

namespace two_lane {
template <typename T, typename Updater> struct data;
template <typename T, typename Updater> struct mutable_reference {
  ~mutable_reference() noexcept {
    try {
      std::unique_lock<std::shared_mutex> l{cache_mutex};
      updater.commit(ref, mutable_ref);
    } catch (...) {
    }
  }

  [[nodiscard]] T &get() { return mutable_ref; }

private:
  friend struct data<T, Updater>;
  mutable_reference(std::mutex &guard, std::shared_mutex &cache_mutex,
                    T &mutable_ref, T &ref, Updater &updater)
      : mutable_ref{mutable_ref}, ref{ref}, guard{guard},
        cache_mutex{cache_mutex}, updater{updater} {}

  T &mutable_ref;
  T &ref;
  std::lock_guard<std::mutex> guard;
  std::shared_mutex &cache_mutex;
  Updater &updater;
};

template <typename T, typename Updater> struct reference {
  [[nodiscard]] const T &get() const { return ref; }

private:
  friend struct data<T, Updater>;
  reference(std::shared_mutex &guard, const T &ref) : ref{ref}, guard{guard} {}

  const T &ref;
  std::shared_lock<std::shared_mutex> guard;
};

struct Move {
  template <typename T> void commit(T &cached, T &modified) {
    std::swap(cached, modified);
  }
};
struct Equal {
  template <typename T> void commit(T &cached, T &modified) {
    cached = modified;
  }
};

template <typename T, typename Updater = Equal> struct data {
  data(T &&board, Updater updater = Equal{})
      : cached{board}, mutdata{std::forward<T>(board)},
        updater{std::forward<Updater>(updater)} {}

  [[nodiscard]] mutable_reference<T, Updater> acquire_mut() {
    return mutable_reference<T, Updater>{mutex, cache_mutex, mutdata, cached,
                                         updater};
  }

  [[nodiscard]] reference<T, Updater> acquire_ref() {
    return reference<T, Updater>{cache_mutex, cached};
  }

private:
  std::shared_mutex cache_mutex;
  std::mutex mutex;
  T cached;
  T mutdata;
  Updater updater;
};
} // namespace two_lane
