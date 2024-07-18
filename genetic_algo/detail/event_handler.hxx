
#pragma once

#include "window_utils.hxx"
#include <SFML/Graphics.hpp>

template <typename State, typename Game>
void EventHandler<State, Game>::handle(const sf::Event &event,
                                       sf::RenderWindow &window) {
  if (event.type == sf::Event::Closed) {
    window.close();
  }
  auto view = window.getView();
  if (event.type == sf::Event::Resized) {
    auto guard = state.acquire_mut();
    auto &board = guard.get();
    board.width = event.size.width;
    board.height = event.size.height;
    board.reset();
    window.setTitle(window_title(state, game));
  }
  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Q) {
      window.close();
    }
    if (event.key.code == sf::Keyboard::R) {
      auto guard = state.acquire_ref();
      const auto &game = guard.get();
      view.setSize({
          static_cast<float>(game.width),
          static_cast<float>(game.height),
      });
      view.setCenter({
          static_cast<float>(game.width) / 2,
          static_cast<float>(game.height) / 2,
      });
      window.setView(view);
    }
    if (event.key.code == sf::Keyboard::S) {
      auto guard = state.acquire_mut();
      auto &game = guard.get();
      game.reset();
    }
  }

  if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == 0) {
      moving = true;
      oldPos = window.mapPixelToCoords(
          sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
    }
  }

  if (event.type == sf::Event::MouseButtonReleased) {
    if (event.mouseButton.button == 0) {
      moving = false;
    }
  }

  if (event.type == sf::Event::MouseMoved) {
    if (!moving)
      return;

    const sf::Vector2f newPos = window.mapPixelToCoords(
        sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
    const sf::Vector2f deltaPos = oldPos - newPos;
    view.move(deltaPos.x, deltaPos.y);
    window.setView(view);
    oldPos = window.mapPixelToCoords(
        sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
  }

  if (event.type == sf::Event::MouseWheelScrolled) {
    if (event.mouseWheelScroll.delta <= -1)
      zoom *= 1.1f;
    else if (event.mouseWheelScroll.delta >= 1)
      zoom /= 1.1f;

    view.setSize(window.getDefaultView().getSize());
    view.zoom(zoom);
    window.setView(view);
  }
}
