#pragma once

#include "../algorithm.hxx"

template <typename State, typename Game>
LogicWidget<State, Game>::LogicWidget(asio::io_context &ctx, State &state,
                                      Game &&game,
                                      std::chrono::milliseconds duration)
    : state{state}, game{std::forward<Game>(game)}, ctx{ctx}, timer{ctx},
      duration{duration} {
  schedule();
}

template <typename State, typename Game>
void LogicWidget<State, Game>::run(const asio::error_code &ec) {
  if (!!ec) {
    return;
  }

  auto board_guard = state.acquire_mut();
  auto &board = board_guard.get();
  const auto &cached = board_guard.get_cached();

  schedule();

  algo::for_each(algo::parallel{ctx}, board.board, [&cached, this](auto &cell) {
    cell = std::move(std::visit(
        [&cached, this](const auto &cell) { return game.step(cell, cached); },
        cell));
  });
}

template <typename State, typename Game>
void LogicWidget<State, Game>::schedule() {
  timer.expires_from_now(duration);
  timer.async_wait(std::bind(&LogicWidget::run, this, std::placeholders::_1));
}
