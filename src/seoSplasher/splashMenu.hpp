
#ifndef SPLASH_MENU_HPP
#define SPLASH_MENU_HPP

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

    bool singlePlayer;
    bool client;
    bool server;
};

#endif
