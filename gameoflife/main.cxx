#include "cell.hxx"
#include "game.hxx"
#include <genetic_algo/draw_widget.hxx>
#include <genetic_algo/drawer.hxx>
#include <genetic_algo/event_handler.hxx>
#include <genetic_algo/logic_widget.hxx>
#include <genetic_algo/scheduler.hxx>
#include <genetic_algo/threadsafe.hxx>
#include <genetic_algo/tracer.hxx>

#include <iostream>

int main(int argc, const char *argv[]) try {
  auto board = state_data::create(argc, argv);
  if (!board)
    return EXIT_SUCCESS;

  asio::io_context main_thread;
  scheduler ctx{main_thread};

  rcu::data state{*std::move(board)};

  Game game;
  LogicWidget logic{ctx, state, game};
  EventHandler event_handler{state, game};
  DrawingWidget draw{ctx, game, state, Drawer{}, std::move(event_handler)};
  auto _ = tracer::instance().report_every(ctx, std::chrono::seconds{1});
  main_thread.run();
  ctx.stop();
  return EXIT_SUCCESS;
} catch (const std::exception &e) {
  std::cerr << "Exception: " << e.what() << "\n";
  return EXIT_FAILURE;
}
