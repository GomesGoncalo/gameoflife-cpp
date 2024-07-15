#include "game.hxx"
#include "cell.hxx"
#include <string>
#include <vector>

unsigned int Game::get(const state_data &data, const point &cell, int8_t xoff,
                       int8_t yoff) const noexcept {
  const auto &[width, height, _, _2, board, _3] = data;
  const auto &[x, y, idx] = cell;
  if (x == 0 && xoff == -1)
    return 0;
  if (x == width - 1 && xoff == 1)
    return 0;
  if (y == 0 && yoff == -1)
    return 0;
  if (y == height - 1 && yoff == 1)
    return 0;

  return std::visit(
      [](const auto &a) noexcept {
        return std::is_same_v<decltype(a), const live &>;
      },
      board[(x + xoff) * height + (y + yoff)]);
}

unsigned int Game::neighbours(const point &cell,
                              const state_data &board) const noexcept {
  return get(board, cell, -1, -1) + get(board, cell, -1, 0) +
         get(board, cell, -1, 1) + get(board, cell, 0, -1) +
         get(board, cell, 0, 1) + get(board, cell, 1, -1) +
         get(board, cell, 1, 0) + get(board, cell, 1, 1);
}

template <>
CellTypes Game::step(const live &cell, const state_data &board) const noexcept {
  const auto neigh = neighbours(cell, board);
  if (neigh == 2 || neigh == 3) {
    return cell;
  }

  return dead{cell};
}

template <>
CellTypes Game::step(const dead &cell, const state_data &board) const noexcept {
  const auto neigh = neighbours(cell, board);
  if (neigh == 3) {
    return live{cell};
  }

  return cell;
}
