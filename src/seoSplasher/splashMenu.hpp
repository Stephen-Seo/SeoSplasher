
#ifndef SPLASH_MENU_HPP
#define SPLASH_MENU_HPP

#define BLINK_TIME 1.0F
#define BLINK_INTERVAL 0.2F

#include "../state.hpp"

#include <SFML/Graphics.hpp>

#include "../gui.hpp"

class SplashMenu : public State
{
public:
    SplashMenu(StateStack& stack, Context context);

    void draw() override;
    bool update(sf::Time dt) override;
    bool handleEvent(const sf::Event& event) override;
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
