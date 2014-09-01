
#ifndef C_SPRITE_HPP
#define C_SPRITE_HPP

#include "../ec/component.hpp"

#include <SFML/Graphics.hpp>

struct cSprite : Component
{
    sf::Sprite sprite;
};

#endif
