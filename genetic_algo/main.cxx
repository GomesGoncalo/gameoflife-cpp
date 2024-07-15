#include "cell.hxx"
#include "draw_widget.hxx"
#include "drawer.hxx"
#include "event_handler.hxx"
#include "game.hxx"
#include "logic_widget.hxx"
#include "scheduler.hxx"
#include "threadsafe.hxx"
#include "variant_utils.hxx"

#include <iostream>

int main(int argc, const char *argv[]) try {
  auto board = state_data::create(argc, argv);
  if (!board)
    return EXIT_SUCCESS;

  asio::io_context ctx;
  OnlyDifferent<VariantDifferentType> updater{ctx};

  two_lane::data<decltype(board)::value_type,
                 OnlyDifferent<VariantDifferentType>>
      state{std::move(board.value()), std::move(updater)};

  scheduler threads{ctx};

  Game game;
  LogicWidget logic{ctx, state, game};

  asio::io_context main_thr;
  DrawingWidget draw{
      main_thr,
      ctx,
      game,
      state,
      Drawer{},
      EventHandler<OnlyDifferent<VariantDifferentType>, Game>{state, game}};
  main_thr.run();
  return EXIT_SUCCESS;
} catch (const std::exception &e) {
  std::cerr << "Exception: " << e.what() << "\n";
  return EXIT_FAILURE;
}
