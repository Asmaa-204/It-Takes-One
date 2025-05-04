#pragma once

#include <entities/world.hpp>

namespace our
{
    class System 
    {
        virtual void update(World* world, float deltaTime) = 0;
    };
}