#pragma once

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <chrono>

template <typename State, typename Game> struct LogicWidget {
  LogicWidget(
      asio::io_context &, State &, Game &&,
      std::chrono::milliseconds duration = std::chrono::milliseconds(10));

private:
  void run(const asio::error_code &ec);
  void schedule();

  State &state;
  Game game;
  asio::io_context &ctx;
  asio::steady_timer timer;
  std::chrono::milliseconds duration;
};

#include "detail/logic_widget.hxx"
