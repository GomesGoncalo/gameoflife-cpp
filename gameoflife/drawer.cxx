#include "cell.hxx"
#include <SFML/Graphics.hpp>
#include <genetic_algo/drawer.hxx>

template <> void Drawer::draw(sf::RenderWindow &window, const live &p) const {
  sf::Vertex point(sf::Vector2f(p.x, p.y), sf::Color::White);
  window.draw(&point, 1, sf::Points);
}

template <> void Drawer::draw(sf::RenderWindow &window, const dead &p) const {}
