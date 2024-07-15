#pragma once

#include "cell.hxx"
#include <cstdint>
#include <iosfwd>

struct Game {
  template <typename T>
  [[nodiscard]] CellTypes step(const T &, const state_data &) const noexcept;

  [[nodiscard]] std::string name() const;

private:
  [[nodiscard]] unsigned int get(const state_data &board, const point &cell,
                                 int8_t xoff, int8_t yoff) const;

  [[nodiscard]] unsigned int neighbours(const point &cell,
                                        const state_data &board) const noexcept;
};
