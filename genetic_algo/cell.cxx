#include "cell.hxx"

#include <random>

state_data::state_data(unsigned int width, unsigned height,
                       unsigned probability)
    : width{width}, height{height}, probability{probability} {
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(0, 100);

  unsigned int idx{0};
  for (auto x = 0u; x < width; ++x) {
    for (auto y = 0u; y < height; ++y) {
      auto prob = dist(rng);
      if (prob < probability) {
        board.push_back(live{x, y, idx});
      } else {
        board.push_back(dead{x, y, idx});
      }

      ++idx;
    }
  }
}
