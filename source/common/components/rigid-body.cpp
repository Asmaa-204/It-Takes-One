#include <components/rigid-body.hpp>
#include <deserialize-utils.hpp>
#include <entities/entity.hpp>
#include <components/mesh-renderer.hpp>
#include <btBulletCollisionCommon.h>

#include <iostream>

namespace our {
    void RigidBodyComponent::deserialize(const nlohmann::json& data)
    {
        if (!data.is_object()) return;

        float mass = data.value("mass", 0.0f);

        Entity* owner = this->getOwner();
        Transform localTransform = owner->localTransform;

        MeshRendererComponent* meshRenderer = owner->getComponent<MeshRendererComponent>();

        btCollisionShape* shape = deepCopyCollisionShape(meshRenderer->mesh->getShape());
        if (!shape) return;

        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(btVector3(
            localTransform.position.x,
            localTransform.position.y,
            localTransform.position.z
        ));
        transform.setRotation(btQuaternion(
            localTransform.rotation.x,
            localTransform.rotation.y,
            localTransform.rotation.z
        ));

        btDefaultMotionState* motionState = new btDefaultMotionState(transform);

        shape->setLocalScaling(btVector3(
            localTransform.scale.x,
            localTransform.scale.y,
            localTransform.scale.z
        ));

        btVector3 localInertia(0,0,0);
        if (mass != 0.0f && shape->getShapeType() != TRIANGLE_MESH_SHAPE_PROXYTYPE) {
            std::cout << "line: " << __LINE__ << '\n';
            std::cout << shape <<'\n';
            shape->calculateLocalInertia(mass, localInertia);
            std::cout << "line: " << __LINE__ << '\n';
        }

        // create the rigid body construction
        // info using the mass, motion state
        // and shape
        btRigidBody::btRigidBodyConstructionInfo cInfo(mass, motionState, shape, localInertia);

        // create the rigid body
	    rigidBody = new btRigidBody(cInfo);
    }    

    btCollisionShape* RigidBodyComponent::deepCopyCollisionShape(const btCollisionShape* original) {
        if (!original) return nullptr;

        btCollisionShape* newShape = nullptr;

        switch(original->getShapeType()) {
            case CONVEX_HULL_SHAPE_PROXYTYPE: {
                const btConvexHullShape* shape = static_cast<const btConvexHullShape*>(original);

                btConvexHullShape* _newShape = new btConvexHullShape();
                for (int i = 0; i < shape->getNumPoints(); i++) {
                    _newShape->addPoint((shape->getScaledPoint(i)));
                }

                return newShape = _newShape;
            }
            case TRIANGLE_MESH_SHAPE_PROXYTYPE: {
                const btBvhTriangleMeshShape* shape = static_cast<const btBvhTriangleMeshShape*>(original);
                
                const btTriangleIndexVertexArray* constMeshInterface = static_cast<const btTriangleIndexVertexArray*>(shape->getMeshInterface());
                btTriangleIndexVertexArray* meshInterface = const_cast<btTriangleIndexVertexArray*>(constMeshInterface);
                newShape = new btBvhTriangleMeshShape(meshInterface, shape->usesQuantizedAabbCompression());

                return newShape;
            }
            default:
                std::cerr << "Unsupported shape type for deep copy: " << original->getShapeType() << '\n';
        }

        return nullptr;
    }
}