
#include "resourceManager.hpp"

ResourceManager::ResourceManager(StateStack* sstack, GameResources::LoadingMode mode, std::string packfile) :
textureHolder(mode, packfile, false),
fontHolder(mode, packfile, true),
soundBufferHolder(mode, packfile, false),
mode(mode),
sstack(sstack)
{}

sf::Texture& ResourceManager::getTexture(Textures::ID id)
{
    return textureHolder.get(id);
}

sf::Font& ResourceManager::getFont(Fonts::ID id)
{
    return fontHolder.get(id);
}

sf::SoundBuffer& ResourceManager::getSoundBuffer(Sound::ID id)
{
    return soundBufferHolder.get(id);
}

void ResourceManager::registerTexture(Textures::ID id, const std::string& filename)
{
    textureHolder.registerResource(id, filename);
}

void ResourceManager::registerFont(Fonts::ID id, const std::string& filename)
{
    fontHolder.registerResource(id, filename);
}

void ResourceManager::registerSoundBuffer(Sound::ID id, const std::string& filename)
{
    soundBufferHolder.registerResource(id, filename);
}

void ResourceManager::loadResources(ResourcesSet rset)
{
    // load textures if not loaded
    if(rset.tset != nullptr)
    {
        for(auto iter : *rset.tset)
        {
            if(!textureHolder.isLoaded(iter))
            {
                textureHolder.load(iter);
            }
        }
    }

    // load fonts if not loaded
    if(rset.fset != nullptr)
    {
        for(auto iter : *rset.fset)
        {
            if(!fontHolder.isLoaded(iter))
            {
                fontHolder.load(iter);
            }
        }
    }

    // load sound buffers if not loaded
    if(rset.sset != nullptr)
    {
        for(auto iter : *rset.sset)
        {
            if(!soundBufferHolder.isLoaded(iter))
            {
                soundBufferHolder.load(iter);
            }
        }
    }
}

void ResourceManager::unloadCheckResources()
{
    ResourcesSet resourcesSet = sstack->getNeededResources();

    // check for unneeded textures
    for(auto iter = textureHolder.resourceMap.begin(); iter != textureHolder.resourceMap.end(); ++iter)
    {
        if(resourcesSet.tset->find(iter->first) == resourcesSet.tset->end())
        {
            iter = --(textureHolder.resourceMap.erase(iter));
        }
    }

    // check for unneeded fonts
    for(auto iter = fontHolder.resourceMap.begin(); iter != fontHolder.resourceMap.end(); ++iter)
    {
        if(resourcesSet.fset->find(iter->first) == resourcesSet.fset->end())
        {
            iter = --(fontHolder.resourceMap.erase(iter));
        }
    }

    // check for unneeded sound buffers
    for(auto iter = soundBufferHolder.resourceMap.begin(); iter != soundBufferHolder.resourceMap.end(); ++iter)
    {
        if(resourcesSet.sset->find(iter->first) == resourcesSet.sset->end())
        {
            iter = --(soundBufferHolder.resourceMap.erase(iter));
        }
    }

    delete resourcesSet.tset;
    delete resourcesSet.fset;
    delete resourcesSet.sset;
}

