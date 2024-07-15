#pragma once

#include "scheduler.hxx"
#include <SFML/Graphics.hpp>
#include <asio/io_context.hpp>

template <typename State, typename Drawer, typename EventHandler, typename Game>
struct DrawingWidget {
  DrawingWidget(scheduler &, Game &, State &, Drawer &&, EventHandler &&);

private:
  void run();
  void process_events();
  void process_drawables();
  void schedule();

  scheduler &ctx;
  sf::RenderWindow window;
  State &state;
  Drawer drawer;
  EventHandler handler;
};

#include "detail/draw_widget.hxx"
