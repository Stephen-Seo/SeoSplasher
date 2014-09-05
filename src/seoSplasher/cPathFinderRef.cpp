
#include "cPathFinderRef.hpp"

#include "cPathFinder.hpp"

cPathFinderRef::cPathFinderRef() :
cpf(nullptr)
{}

cPathFinderRef::cPathFinderRef(cPathFinder* cpf) :
cpf(cpf)
{}

