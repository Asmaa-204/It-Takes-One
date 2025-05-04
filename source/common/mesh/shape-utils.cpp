
#include "shape-utils.hpp"
#include <iostream>
namespace our
{

    btCollisionShape *generateBtBvhTriangleMeshShape(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &elements)
    {
        int elementCount = elements.size();
        btTriangleIndexVertexArray *data = new btTriangleIndexVertexArray;
        // add an empty mesh (data makes a copy)
        btIndexedMesh tempMesh;
        data->addIndexedMesh(tempMesh, PHY_FLOAT);
        // get a reference to internal copy of the empty mesh
        btIndexedMesh &mesh = data->getIndexedMeshArray()[0];
        // allocate memory for the mesh
        mesh.m_numTriangles = elementCount / 3;
        if (elementCount < std::numeric_limits<int16_t>::max())
        {
            // we can use 16-bit indices
            mesh.m_triangleIndexBase = new unsigned char[sizeof(int16_t) * (size_t)elementCount];
            mesh.m_indexType = PHY_SHORT;
            mesh.m_triangleIndexStride = 3 * sizeof(int16_t);
        }
        else
        {
            // we need 32-bit indices
            mesh.m_triangleIndexBase = new unsigned char[sizeof(int32_t) * (size_t)elementCount];
            mesh.m_indexType = PHY_INTEGER;
            mesh.m_triangleIndexStride = 3 * sizeof(int32_t);
        }
        mesh.m_numVertices = vertices.size();
        mesh.m_vertexBase = new unsigned char[3 * sizeof(btScalar) * (size_t)mesh.m_numVertices];
        mesh.m_vertexStride = 3 * sizeof(btScalar);

        // copy vertices into mesh
        btScalar *vertexData = static_cast<btScalar *>((void *)(mesh.m_vertexBase));
        for (int32_t i = 0; i < mesh.m_numVertices; ++i)
        {
            int32_t j = i * 3;
            const glm::vec3 &point = vertices[i].position;
            vertexData[j] = point.x;
            vertexData[j + 1] = point.y;
            vertexData[j + 2] = point.z;
        }
        // copy indices into mesh
        if (elementCount < std::numeric_limits<int16_t>::max())
        {
            // 16-bit case
            int16_t *indices = static_cast<int16_t *>((void *)(mesh.m_triangleIndexBase));
            for (int32_t i = 0; i < elementCount; ++i)
            {
                indices[i] = (int16_t)elements[i];
            }
        }
        else
        {
            // 32-bit case
            int32_t *indices = static_cast<int32_t *>((void *)(mesh.m_triangleIndexBase));
            for (int32_t i = 0; i < elementCount; ++i)
            {
                indices[i] = elements[i];
            }
        }

        // create the shape
        // NOTE: we must track this pointer and delete it when all btCollisionObjects that use it are done with it!
        const bool USE_QUANTIZED_AABB_COMPRESSION = true;
        return new btBvhTriangleMeshShape(data, USE_QUANTIZED_AABB_COMPRESSION);
    }

    btCollisionShape* generateBtConvexHullShape(const std::vector<Vertex> &vertices) {
        btConvexHullShape* convexShape = new btConvexHullShape();
        
        for(int i = 0; i < vertices.size(); ++i) {
            const Vertex& v = vertices[i];
            convexShape->addPoint(btVector3(v.position.x, v.position.y, v.position.z));
        }
        
        // Optional: Optimize the convex hull
        convexShape->initializePolyhedralFeatures();
        convexShape->optimizeConvexHull();
        return convexShape;
    }

}
