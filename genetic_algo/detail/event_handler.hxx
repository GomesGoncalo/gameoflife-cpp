
#pragma once

#include <SFML/Graphics.hpp>

template <typename Updater>
void EventHandler<Updater>::handle(const sf::Event &event,
                                   sf::RenderWindow &window) const {
  if (event.type == sf::Event::Closed) {
    window.close();
  }
  auto view = window.getView();
  if (event.type == sf::Event::Resized) {
    view.setSize({
        static_cast<float>(event.size.width),
        static_cast<float>(event.size.height),
    });
    window.setView(view);
    auto guard = state.acquire_mut();
    auto &board = guard.get();
    board = state_data(event.size.width, event.size.height, board.probability);
  }
  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Q) {
      window.close();
    }

    if (event.key.code == sf::Keyboard::J) {
      view.zoom(1.1);
      window.setView(view);
    }
    if (event.key.code == sf::Keyboard::K) {
      view.zoom(0.9);
      window.setView(view);
    }
    if (event.key.code == sf::Keyboard::Up) {
      view.move(0, -10);
      window.setView(view);
    }
    if (event.key.code == sf::Keyboard::Down) {
      view.move(0, 10);
      window.setView(view);
    }
    if (event.key.code == sf::Keyboard::Left) {
      view.move(-10, 0);
      window.setView(view);
    }
    if (event.key.code == sf::Keyboard::Right) {
      view.move(10, 0);
      window.setView(view);
    }
  }
}
