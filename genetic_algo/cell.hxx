#pragma once

#include <variant>
#include <vector>

struct point {
  unsigned int x;
  unsigned int y;
};

struct live final : public point {};
struct dead final : public point {};

using CellTypes = std::variant<dead, live>;

struct state_data {
  state_data(unsigned int width, unsigned height, unsigned prob);

  unsigned int width = 1000;
  unsigned int height = 1000;
  unsigned int probability = 70;
  std::vector<CellTypes> board;
};
