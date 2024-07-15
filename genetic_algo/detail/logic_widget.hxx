#pragma once

#include "../algorithm.hxx"

template <typename State, typename Game>
LogicWidget<State, Game>::LogicWidget(scheduler &ctx, State &state, Game &game)
    : state{state}, game{game}, ctx{ctx}, timer{ctx} {
  auto state_guard = state.acquire_ref();
  const auto &state_ref = state_guard.get();

  generations = state_ref.generations;

  schedule();
}

template <typename State, typename Game>
void LogicWidget<State, Game>::run(const asio::error_code &ec) {
  if (!!ec) {
    return;
  }

  auto board_mut_guard = state.acquire_mut();
  auto &board_mut = board_mut_guard.get();
  auto board_guard = state.acquire_ref();
  auto &cached = board_guard.get();

  auto guard = tracer::instance().acquire("logic");
  algo::for_each(algo::parallel{ctx}, board_mut.board,
                 [&cached, this](auto &cell) {
                   cell = std::move(std::visit(
                       [&cached, this](const auto &cell) {
                         return game.step(cell, cached);
                       },
                       cell));
                 });
  schedule();
}

template <typename State, typename Game>
void LogicWidget<State, Game>::schedule() {
  if (generations && current_generation++ == generations) {
    ctx.stop();
    return;
  }

  auto state_guard = state.acquire_ref();
  const auto &state_ref = state_guard.get();
  timer.expires_from_now(state_ref.logic_granularity);
  timer.async_wait(std::bind(&LogicWidget::run, this, std::placeholders::_1));
}
