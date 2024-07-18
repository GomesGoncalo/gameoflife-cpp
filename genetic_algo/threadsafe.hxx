#pragma once

#include <mutex>
#include <shared_mutex>

namespace rcu {
template <typename T, typename Copy, typename Update> struct data;
template <typename T, typename Copy, typename Update> struct mutable_reference {
  ~mutable_reference() noexcept try {
    std::unique_lock<std::shared_mutex> l{cache_mutex};
    updater.update(ref, mutable_ref);
  } catch (...) {
  }

  mutable_reference(mutable_reference &&) = delete;
  mutable_reference(const mutable_reference &) = delete;
  mutable_reference &operator=(mutable_reference &&) = delete;
  mutable_reference &operator=(const mutable_reference &) = delete;

  [[nodiscard]] T &get() noexcept { return mutable_ref; }

private:
  friend struct data<T, Copy, Update>;
  mutable_reference(std::mutex &lock, std::shared_mutex &cache_mutex,
                    T &mutable_ref, T &ref, Copy &copier, Update &updater)
      : mutable_ref{mutable_ref}, ref{ref}, lock{lock},
        cache_mutex{cache_mutex}, updater{updater} {
    std::shared_lock<std::shared_mutex> guard{cache_mutex};
    copier.copy(mutable_ref, ref);
  }

  T &mutable_ref;
  T &ref;
  std::lock_guard<std::mutex> lock;
  std::shared_mutex &cache_mutex;
  Update &updater;
};

template <typename T, typename Copy, typename Update> struct reference {
  [[nodiscard]] const T &get() const noexcept { return ref; }

private:
  friend struct data<T, Copy, Update>;
  reference(std::shared_mutex &guard, const T &ref) : ref{ref}, guard{guard} {}

  const T &ref;
  std::shared_lock<std::shared_mutex> guard;
};

struct Update {
  template <typename T> void update(T &to, T &from) const {
    std::swap(to, from);
  }
};

struct Copy {
  template <typename T> void copy(T &to, T &from) const { to = from; }
};

template <typename T, typename Copy = Copy, typename Update = Update>
struct data {
  data(T &&board, Copy copier = Copy{}, Update updater = Update{})
      : cached{std::forward<T>(board)}, mutdata{},
        copier{std::forward<Copy>(copier)},
        updater{std::forward<Update>(updater)} {}

  [[nodiscard]] mutable_reference<T, Copy, Update> acquire_mut() {
    return mutable_reference<T, Copy, Update>{mutex,  cache_mutex, mutdata,
                                              cached, copier,      updater};
  }

  [[nodiscard]] reference<T, Copy, Update> acquire_ref() const {
    return reference<T, Copy, Update>{cache_mutex, cached};
  }

private:
  mutable std::shared_mutex cache_mutex;
  std::mutex mutex;
  T cached;
  T mutdata;
  Copy copier;
  Update updater;
};
} // namespace rcu
