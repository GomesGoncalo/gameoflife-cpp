#pragma once

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

  state_data clone() const;
  void reset();

  static std::optional<state_data> create(int argc, const char *argv[]);

  unsigned int width = 1000;
  unsigned int height = 1000;
  unsigned int probability = 25;
  std::chrono::milliseconds logic_granularity{30};
  std::vector<CellTypes> board;
  uint64_t generations{0};
};
