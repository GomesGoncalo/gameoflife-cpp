#pragma once

namespace sf {
struct RenderWindow;
}

struct Drawer {
  template <typename T> void draw(sf::RenderWindow &, const T &) const;
};
