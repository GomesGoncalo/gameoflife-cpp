#pragma once

#include <asio/post.hpp>

template <typename State, typename Drawer, typename EventHandler>
DrawingWidget<State, Drawer, EventHandler>::DrawingWidget(
    asio::io_context &main_ctx, asio::io_context &ctx, const std::string &name,
    State &state, Drawer &&drawer, EventHandler &&handler)
    : main_ctx{main_ctx}, ctx{ctx}, window{[&name, &state] {
        const auto guard = state.acquire_ref();
        const auto &ref = guard.get();
        return sf::RenderWindow{sf::VideoMode{ref.width, ref.height}, name};
      }()},
      state(state), drawer{std::forward<Drawer>(drawer)},
      handler{std::forward<EventHandler>(handler)} {
  window.setVerticalSyncEnabled(true);
  schedule();
}

template <typename State, typename Drawer, typename EventHandler>
void DrawingWidget<State, Drawer, EventHandler>::run() {
  if (!window.isOpen()) {
    ctx.stop();
    return;
  }

  process_events();
  process_drawables();
  schedule();
}

template <typename State, typename Drawer, typename EventHandler>
void DrawingWidget<State, Drawer, EventHandler>::process_events() {
  sf::Event event;
  while (window.isOpen() && window.pollEvent(event)) {
    handler.handle(event, window);
  }
}

template <typename State, typename Drawer, typename EventHandler>
void DrawingWidget<State, Drawer, EventHandler>::process_drawables() {
  if (!window.isOpen())
    return;

  window.clear(sf::Color::Black);

  auto news = state.acquire_ref();
  for (const auto &val : news.get().board) {
    std::visit([this](const auto &p) { drawer.draw(window, p); }, val);
  }

  window.display();
}

template <typename State, typename Drawer, typename EventHandler>
void DrawingWidget<State, Drawer, EventHandler>::schedule() {
  if (!window.isOpen())
    return;

  asio::post(main_ctx, std::bind(&DrawingWidget::run, this));
}
