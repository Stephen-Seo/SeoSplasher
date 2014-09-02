
#ifndef C_ANIMATED_HPP
#define C_ANIMATED_HPP

#define DEFAULT_FRAME_TIME 0.4f

#include "../ec/component.hpp"

#include <vector>

#include <SFML/Graphics.hpp>

struct cAnimated : Component
{
    enum Type {
        CYCLE,
        REPEAT
    };
    std::vector<sf::Texture*> textures;
    char current = 0;
    bool reverse = false;

    Type type = CYCLE;
    float frameTime = DEFAULT_FRAME_TIME;
    float timer = DEFAULT_FRAME_TIME;
};

#endif
