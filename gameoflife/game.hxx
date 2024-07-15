#pragma once

#include "cell.hxx"
#include <cstdint>
#include <iosfwd>

struct Game {
  template <typename T>
  [[nodiscard]] CellTypes step(const T &, const state_data &) const noexcept;

  [[nodiscard]] constexpr std::string name() const { return "Game of Life"; }

private:
  [[nodiscard]] unsigned int get(const state_data &board, const point &cell,
                                 int8_t xoff, int8_t yoff) const noexcept;

  [[nodiscard]] unsigned int neighbours(const point &cell,
                                        const state_data &board) const noexcept;
};
