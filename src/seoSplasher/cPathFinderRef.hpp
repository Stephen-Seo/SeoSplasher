
#ifndef C_PATH_FINDER_REF_HPP
#define C_PATH_FINDER_REF_HPP

#include "../ec/component.hpp"

struct cPathFinder;

struct cPathFinderRef : Component
{
    cPathFinderRef();
    cPathFinderRef(cPathFinder* cpf);

    cPathFinder* cpf;
};

#endif
