#pragma once

template <typename State, typename Game>
std::string window_title(State &state, Game &game) {
  auto guard = state.acquire_ref();
  const auto &bstate = guard.get();
  std::string name = game.name();
  name += " " + std::to_string(bstate.height * bstate.width);
  return name;
}
