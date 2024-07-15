#include "cell.hxx"
#include "game.hxx"
#include <genetic_algo/draw_widget.hxx>
#include <genetic_algo/drawer.hxx>
#include <genetic_algo/event_handler.hxx>
#include <genetic_algo/logic_widget.hxx>
#include <genetic_algo/scheduler.hxx>
#include <genetic_algo/threadsafe.hxx>
#include <genetic_algo/tracer.hxx>
#include <genetic_algo/variant_utils.hxx>

#include <asio/signal_set.hpp>
#include <iostream>

int main(int argc, const char *argv[]) try {
  auto board = state_data::create(argc, argv);
  if (!board)
    return EXIT_SUCCESS;

  scheduler ctx;
  OnlyDifferent<VariantSameType> updater{ctx};

  two_lane::data<decltype(board)::value_type, OnlyDifferent<VariantSameType>>
      state{*std::move(board), std::move(updater)};

  Game game;
  LogicWidget logic{ctx, state, game};

  DrawingWidget draw{ctx, game, state, Drawer{},
                     EventHandler<decltype(state), Game>{state, game}};
  auto _ = tracer::instance().report_every(ctx, std::chrono::seconds{1});
  ctx.run();
  return EXIT_SUCCESS;
} catch (const std::exception &e) {
  std::cerr << "Exception: " << e.what() << "\n";
  return EXIT_FAILURE;
}
