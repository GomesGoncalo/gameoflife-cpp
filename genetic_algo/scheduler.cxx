#include "scheduler.hxx"
#include <thread>

scheduler::scheduler(asio::io_context &main_ctx) : main_ctx{main_ctx} {
  const auto nthreads = std::thread::hardware_concurrency();
  threads.reserve(nthreads);
  for (unsigned int start = 0u; start < nthreads; ++start) {
    threads.emplace_back([this] {
      auto guard = asio::make_work_guard(*this);
      asio::io_context::run();
    });
  }
}
scheduler::~scheduler() {
  stop();
  for (auto &t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
}
