#pragma once

#include "../algorithm.hxx"

template <typename State, typename Game>
LogicWidget<State, Game>::LogicWidget(asio::io_context &ctx, State &state,
                                      Game &game)
    : state{state}, game{game}, ctx{ctx}, timer{ctx} {
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
  auto state_guard = state.acquire_ref();
  const auto &state_ref = state_guard.get();
  timer.expires_from_now(state_ref.logic_granularity);
  timer.async_wait(std::bind(&LogicWidget::run, this, std::placeholders::_1));
}
