#pragma once

#include "cell.hxx"
#include "threadsafe.hxx"
namespace sf {
struct RenderWindow;
struct Event;
} // namespace sf

template <typename Updater> struct EventHandler {
  EventHandler(two_lane::data<state_data, Updater> &state) : state{state} {}
  void handle(const sf::Event &event, sf::RenderWindow &window) const;

private:
  two_lane::data<state_data, Updater> &state;
};

#include "detail/event_handler.hxx"
