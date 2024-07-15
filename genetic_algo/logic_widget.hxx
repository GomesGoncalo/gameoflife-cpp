#pragma once

#include "scheduler.hxx"
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <chrono>

template <typename State, typename Game> struct LogicWidget {
  LogicWidget(scheduler &, State &, Game &);

private:
  void run(const asio::error_code &ec);
  void schedule();

  State &state;
  Game &game;
  scheduler &ctx;
  asio::steady_timer timer;
  std::chrono::milliseconds duration;
  uint64_t current_generation{0};
  uint64_t generations{0};
};

#include "detail/logic_widget.hxx"
