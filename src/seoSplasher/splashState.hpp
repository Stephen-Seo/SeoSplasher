
#ifndef SPLASH_STATE_HPP
#define SPLASH_STATE_HPP

#include "../state.hpp"

#include <random>

#include "cPlayerControl.hpp"
#include "cPowerup.hpp"
#include "direction.hpp"

class SplashState : public State
{
public:
    SplashState(StateStack& stack, Context context);

    void draw();
    bool update(sf::Time dt);
    bool handleEvent(const sf::Event& event);
private:
    sf::RectangleShape fieldBG;

    unsigned char IDcounter;

    Direction::Direction dir;
    bool placeBalloon;
    bool placeAction;
    bool kick;
    bool kickAction;

    bool wUp, aLeft, sDown, dRight;

    bool cFired;

    void addWall(float x, float y);
    void addCombatant(bool isPlayer);
    void checkReleasedInput();
    void addBreakable(float x, float y, cPowerup::Powerup powerup = cPowerup::NONE);
    void initBreakables();
    bool validBreakableCoordinate(int x, int y);
};

#endif
