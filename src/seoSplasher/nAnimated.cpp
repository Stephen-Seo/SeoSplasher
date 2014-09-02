
#include "nAnimated.hpp"

#include <typeindex>

#include "cAnimated.hpp"
#include "cSprite.hpp"
#include "cTimer.hpp"
#include "../context.hpp"

nAnimated::nAnimated() :
sprites(nullptr),
sprite(nullptr),
timer(nullptr)
{}

bool nAnimated::checkEntity(Entity& entity)
{
    return entity.hasComponent(std::type_index(typeid(cAnimated))) &&
           entity.hasComponent(std::type_index(typeid(cSprite)));
}

std::unique_ptr<Node> nAnimated::getNewNode()
{
    return std::unique_ptr<Node>(new nAnimated);
}

void nAnimated::getCReferencesFromEntity(Entity& entity)
{
    sprites = static_cast<cAnimated*>(entity.getComponent(std::type_index(typeid(cAnimated))));
    sprite = static_cast<cSprite*>(entity.getComponent(std::type_index(typeid(cSprite))));
    entityRemoved = &entity.removed;
}

void nAnimated::update(sf::Time dt, Context context)
{
    if(*entityRemoved)
        return;

    sprites->timer -= dt.asSeconds();
    if(sprites->timer <= 0.0f)
    {
        sprites->timer = sprites->frameTime;
        if(sprites->type == cAnimated::REPEAT && sprites->textures.size() > 0)
        {
            sprites->current = (sprites->current + 1) % sprites->textures.size();
        }
        else if(sprites->type == cAnimated::CYCLE && sprites->textures.size() > 1)
        {
            if(sprites->reverse)
            {
                --(sprites->current);
                if(sprites->current < 0)
                {
                    sprites->current = 1;
                    sprites->reverse = false;
                }
            }
            else
            {
                ++(sprites->current);
                if(sprites->current >= sprites->textures.size())
                {
                    sprites->current -= 2;
                    sprites->reverse = true;
                }
            }
        }
        sprite->sprite.setTexture(*(sprites->textures[sprites->current]));
    }
}
