
#include "context.hpp"

Context::Context(sf::RenderWindow& window, ResourceManager& resourceManager, MusicPlayer& mPlayer, SoundPlayer& sPlayer, Engine& ecEngine, std::mt19937& rGen, bool& isQuitting, unsigned char& mode, ServerContext& scontext) :
window(&window),
resourceManager(&resourceManager),
mPlayer(&mPlayer),
sPlayer(&sPlayer),
ecEngine(&ecEngine),
rGen(&rGen),
isQuitting(&isQuitting),
mode(&mode),
scontext(&scontext)
{}
