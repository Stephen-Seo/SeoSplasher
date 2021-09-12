
#include "resourceIdentifiers.hpp"

ResourcesSet::ResourcesSet() :
tset(nullptr),
fset(nullptr),
sset(nullptr)
{}

ResourcesSet::ResourcesSet(TextureSet& tset, FontSet& fset, SoundSet& sset) :
tset(&tset),
fset(&fset),
sset(&sset)
{}
