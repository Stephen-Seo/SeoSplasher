
#ifndef SPLASH_STATE_HPP
#define SPLASH_STATE_HPP

#include "../state.hpp"

class SplashState : public State
{
public:
    SplashState(StateStack& stack, Context context);

    void draw();
    bool update(sf::Time dt);
    bool handleEvent(const sf::Event& event);
private:
    sf::RectangleShape fieldBG;

    void addWall(float x, float y);
};

#endif
