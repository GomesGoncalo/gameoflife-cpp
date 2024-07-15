#pragma once

#include "algorithm.hxx"

#include <asio/io_context.hpp>
#include <chrono>
#include <variant>
#include <vector>

struct point {
  unsigned int x;
  unsigned int y;
  unsigned int idx;
};

struct live final : public point {};
struct dead final : public point {};

using CellTypes = std::variant<dead, live>;

struct state_data {
  state_data() = default;
  state_data(unsigned int width, unsigned height, unsigned prob,
             std::chrono::milliseconds msec);

  void reset();

  static std::optional<state_data> create(int argc, const char *argv[]);

  unsigned int width = 1000;
  unsigned int height = 1000;
  unsigned int probability = 25;
  std::chrono::milliseconds logic_granularity{30};
  std::vector<CellTypes> board;
};

template <typename Inequality> struct OnlyDifferent {
  OnlyDifferent(asio::io_context &ctx) : ctx{ctx} {}

  void commit(state_data &cached, const state_data &modified) {
    if (cached.width != modified.width || cached.height != modified.height ||
        cached.probability != modified.probability ||
        cached.logic_granularity != modified.logic_granularity) {
      cached = modified;
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

private:
  asio::io_context &ctx;
};
