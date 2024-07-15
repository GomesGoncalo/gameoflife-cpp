#include "algorithm.hxx"
#include "cell.hxx"
#include "draw_widget.hxx"
#include "drawer.hxx"
#include "event_handler.hxx"
#include "game.hxx"
#include "logic_widget.hxx"
#include "scheduler.hxx"
#include "threadsafe.hxx"

#include <ranges>
#include <type_traits>

struct VariantDifferentType {
  template <typename... Args>
  bool cmp(const std::variant<Args...> &l,
           const std::variant<Args...> &r) const noexcept {
    return std::visit(
        [](const auto &l, const auto &r) noexcept {
          return !std::is_same_v<decltype(l), decltype(r)>;
        },
        l, r);
  }
};

template <typename Inequality> struct OnlyDifferent {
  template <typename T> void commit(T &cached, const T &modified) {
    if (cached.width != modified.width || cached.height != modified.height) {
      cached.width = modified.width;
      cached.height = modified.height;
      cached.board = modified.board;
    } else {
      Inequality neq;
      algo::for_each(
          algo::parallel{ctx}, cached.board, [&modified, &neq](auto &element) {
            const auto idx =
                std::visit([](const auto &p) { return p.idx; }, element);
            auto &modified_element = modified.board[idx];
            if (!neq.cmp(element, modified_element)) {
              return;
            }
            element = modified_element;
          });
    }
  }
  asio::io_context &ctx;
};

int main(int argc, char *argv[]) {
  state_data board;

  asio::io_context ctx;
  OnlyDifferent<VariantDifferentType> updater{ctx};

  two_lane::data<decltype(board), OnlyDifferent<VariantDifferentType>> state{
      std::move(board), std::move(updater)};

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
  return 0;
}
