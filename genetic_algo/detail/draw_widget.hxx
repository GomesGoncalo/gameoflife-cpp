#pragma once

#include "../tracer.hxx"
#include "window_utils.hxx"
#include <asio/post.hpp>

template <typename State, typename Drawer, typename EventHandler, typename Game>
DrawingWidget<State, Drawer, EventHandler, Game>::DrawingWidget(
    scheduler &ctx, Game &game, State &state, Drawer &&drawer,
    EventHandler &&handler)
    : ctx{ctx}, window{[&game, &state] {
        const auto guard = state.acquire_ref();
        const auto &ref = guard.get();
        return sf::RenderWindow{sf::VideoMode{ref.width, ref.height},
                                window_title(state, game)};
      }()},
      state(state), drawer{std::forward<Drawer>(drawer)},
      handler{std::forward<EventHandler>(handler)} {
  window.setFramerateLimit(0);
  window.setVerticalSyncEnabled(true);
  schedule();
}

template <typename State, typename Drawer, typename EventHandler, typename Game>
void DrawingWidget<State, Drawer, EventHandler, Game>::run() {
  process_events();
  process_drawables();
}

template <typename State, typename Drawer, typename EventHandler, typename Game>
void DrawingWidget<State, Drawer, EventHandler, Game>::process_events() {
  auto guard = tracer::instance().acquire("event");
  sf::Event event;
  while (window.isOpen() && window.pollEvent(event)) {
    handler.handle(event, window);
  }
}

template <typename State, typename Drawer, typename EventHandler, typename Game>
void DrawingWidget<State, Drawer, EventHandler, Game>::process_drawables() {
  if (!window.isOpen()) {
    return;
  }

  window.clear(sf::Color::Black);

  {
    auto guard = tracer::instance().acquire("draw");
    auto news = state.acquire_ref();
    for (const auto &val : news.get().board) {
      std::visit([this](const auto &p) { drawer.draw(window, p); }, val);
    }
  }

  window.display();
}

template <typename State, typename Drawer, typename EventHandler, typename Game>
void DrawingWidget<State, Drawer, EventHandler, Game>::schedule() {
  if (!window.isOpen()) {
    ctx.stop();
    return;
  }

  asio::post(ctx.main_thread(), [this] {
    run();
    schedule();
  });
}
