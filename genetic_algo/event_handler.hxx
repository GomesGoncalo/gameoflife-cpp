#pragma once

#include "cell.hxx"
#include "threadsafe.hxx"
namespace sf {
struct RenderWindow;
struct Event;
} // namespace sf

template <typename Updater, typename Game> struct EventHandler {
  EventHandler(two_lane::data<state_data, Updater> &state, Game &game)
      : state{state}, game{game} {}
  void handle(const sf::Event &event, sf::RenderWindow &window) const;

private:
  two_lane::data<state_data, Updater> &state;
  Game &game;
};

#include "detail/event_handler.hxx"
