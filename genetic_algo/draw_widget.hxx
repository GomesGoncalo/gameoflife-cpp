#pragma once

#include <SFML/Graphics.hpp>
#include <asio/io_context.hpp>

template <typename State, typename Drawer, typename EventHandler, typename Game>
struct DrawingWidget {
  DrawingWidget(asio::io_context &, asio::io_context &, Game &, State &,
                Drawer &&, EventHandler &&);

private:
  void run();
  void process_events();
  void process_drawables();
  void schedule();

  asio::io_context &main_ctx;
  asio::io_context &ctx;
  sf::RenderWindow window;
  State &state;
  Drawer drawer;
  EventHandler handler;
};

#include "detail/draw_widget.hxx"
