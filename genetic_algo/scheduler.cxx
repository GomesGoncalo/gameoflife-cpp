#include "scheduler.hxx"

scheduler::scheduler(asio::io_context &ctx, const unsigned int num)
    : ctx{ctx}, work{asio::make_work_guard(ctx)} {
  threads.reserve(num);
  for (std::remove_cvref_t<decltype(num)> start = 0; start < num; ++start) {
    threads.emplace_back([&ctx] { ctx.run(); });
  }
}

scheduler::~scheduler() {
  work.reset();
  ctx.stop();
}
