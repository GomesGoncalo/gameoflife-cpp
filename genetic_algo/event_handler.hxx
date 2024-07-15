#pragma once

namespace sf {
struct RenderWindow;
struct Event;
} // namespace sf

template <typename State, typename Game> struct EventHandler {
  EventHandler(State &state, Game &game) : state{state}, game{game} {}
  void handle(const sf::Event &event, sf::RenderWindow &window) const;

private:
  State &state;
  Game &game;
};

#include "detail/event_handler.hxx"
