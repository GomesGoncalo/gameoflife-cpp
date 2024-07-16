#pragma once

namespace sf {
struct RenderWindow;
struct Event;
} // namespace sf

template <typename State, typename Game> struct EventHandler {
  EventHandler(State &state, Game &game) : state{state}, game{game} {}
  void handle(const sf::Event &event, sf::RenderWindow &window);

private:
  State &state;
  Game &game;
  sf::Vector2f oldPos;
  bool moving = false;
  float zoom = 1;
};

#include "detail/event_handler.hxx"
