#pragma once

#include <btBulletCollisionCommon.h>
#include "vertex.hpp"

namespace our
{

    btCollisionShape *generateBtBvhTriangleMeshShape(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &elements);

    btCollisionShape* generateBtConvexHullShape(const std::vector<Vertex> &vertices);

}
