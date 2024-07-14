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
      std::ranges::for_each(std::views::zip(cached.board, modified.board) |
                                std::views::filter([&neq](auto elem) {
                                  return neq.cmp(elem.first, elem.second);
                                }),
                            [](auto elem) noexcept {
                              auto &[cached, modified] = elem;
                              cached = modified;
                            });
    }
  }
};

int main(int argc, char *argv[]) {
  state_data board(1000, 1000, 70);

  asio::io_context ctx;
  OnlyDifferent<VariantDifferentType> updater;

  two_lane::data<decltype(board), OnlyDifferent<VariantDifferentType>> state{
      std::move(board), std::move(updater)};

  scheduler threads{ctx};

  LogicWidget logic{ctx, state, Game{}};

  asio::io_context main_thr;
  DrawingWidget draw{
      main_thr,       ctx,
      "Game of Life", state,
      Drawer{},       EventHandler<OnlyDifferent<VariantDifferentType>>{state}};
  main_thr.run();
  return 0;
}
