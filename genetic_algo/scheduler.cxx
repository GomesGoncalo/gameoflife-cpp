#include "scheduler.hxx"

void scheduler::run() {
  std::vector<std::thread> threads;
  threads.reserve(size_);
  auto guard = asio::make_work_guard(*this);
  for (unsigned int start = 0u; start < size_; ++start) {
    threads.emplace_back([this] { asio::io_context::run(); });
  }
  main_ctx.run();
  guard.reset();
  asio::io_context::stop();
  for (auto &t : threads) {
    if (t.joinable())
      t.join();
  }
}
