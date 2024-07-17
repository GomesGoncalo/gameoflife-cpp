#include "cell.hxx"
#include <SFML/Graphics.hpp>
#include <genetic_algo/drawer.hxx>

template <> void Drawer::draw(sf::RenderWindow &window, const live &p) const {
  sf::RectangleShape rectangle(sf::Vector2f(1, 1));
  rectangle.setFillColor(sf::Color::White);
  rectangle.setPosition(sf::Vector2f(p.x, p.y));
  window.draw(rectangle);
}

template <> void Drawer::draw(sf::RenderWindow &window, const dead &p) const {}
