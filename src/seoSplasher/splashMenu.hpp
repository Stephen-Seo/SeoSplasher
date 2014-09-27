
#ifndef SPLASH_MENU_HPP
#define SPLASH_MENU_HPP

#define BLINK_TIME 1.0f
#define BLINK_INTERVAL 0.2f

#include "../state.hpp"

#include <SFML/Graphics.hpp>

#include "../gui.hpp"

class SplashMenu : public State
{
public:
    SplashMenu(StateStack& stack, Context context);

    void draw();
    bool update(sf::Time dt);
    bool handleEvent(const sf::Event& event);
private:
    GuiSystem guiSystem;

    sf::Text IPText;
    sf::Text infoIPText;

    bool singlePlayer;
    bool client;
    bool server;

    float blinkTime;
    float blinkInterval;
    bool blinking;
};

#endif
