
#include "stateStack.hpp"

StateStack::PendingChange::PendingChange(Action action, States::ID stateID)
: action(action), stateID(stateID)
{
}

StateStack::StateStack(Context context)
: stack(), pendingList(), context(context), factories()
{
}

void StateStack::update(sf::Time dt)
{
    for(auto iter = stack.rbegin(); iter != stack.rend(); ++iter)
    {
        if(!(*iter)->update(dt))
            break;
    }
    applyPendingChanges();
}

void StateStack::draw()
{
    std::for_each(stack.rbegin(), stack.rend(), [] (State::Ptr& state)
    {
        state->draw();
    } );
}

void StateStack::handleEvent(const sf::Event& event)
{
    for(auto iter = stack.rbegin(); iter != stack.rend(); ++iter)
    {
        if(!(*iter)->handleEvent(event))
            break;
    }
    applyPendingChanges();
}

void StateStack::pushState(States::ID stateID)
{
    pendingList.emplace_back(StateStack::Push, stateID);
}

void StateStack::popState()
{
    pendingList.emplace_back(StateStack::Pop);
}

void StateStack::clearStates()
{
    pendingList.emplace_back(StateStack::Clear);
}

bool StateStack::isEmpty() const
{
    return stack.empty();
}

ResourcesSet StateStack::getNeededResources()
{
    ResourcesSet resourcesSet;
    resourcesSet.tset = new TextureSet();
    resourcesSet.fset = new FontSet();
    resourcesSet.sset = new SoundSet();

    for(auto & iter : stack)
    {
        ResourcesSet stateSet = iter->getNeededResources();
        if(stateSet.tset != nullptr)
        {
            for(auto siter : *stateSet.tset)
            {
                resourcesSet.tset->insert(siter);
            }
        }

        if(stateSet.fset != nullptr)
        {
            for(auto siter : *stateSet.fset)
            {
                resourcesSet.fset->insert(siter);
            }
        }

        if(stateSet.sset != nullptr)
        {
            for(auto siter : *stateSet.sset)
            {
                resourcesSet.sset->insert(siter);
            }
        }
    }

    return resourcesSet;
}

State::Ptr StateStack::createState(States::ID stateID)
{
    auto found = factories.find(stateID);
    assert(found != factories.end());
    return found->second();
}

void StateStack::applyPendingChanges()
{
    std::for_each(pendingList.begin(), pendingList.end(), [this] (PendingChange& change)
    {
        switch (change.action)
        {
        case Push:
            stack.push_back(createState(change.stateID));
            break;
        case Pop:
            stack.pop_back();
            break;
        case Clear:
            stack.clear();
            break;
        }
    } );
    pendingList.clear();
}
