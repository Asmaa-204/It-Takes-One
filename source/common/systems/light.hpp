#include <entities/world.hpp>
#include <components/camera.hpp>
#include <components/light.hpp>
#include <components/mesh-renderer.hpp>
#include <application.hpp>
#include <iostream>

#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

namespace our{

    class LightSystem
    {
    private: 
        void setLightingsUniforms(ShaderProgram* shader, Entity* light, int index)
        {
            std::string prefix = "lights[" + std::to_string(index) + "].";
            LightComponent* l = light->getComponent<LightComponent>();

            glm::vec3 direction = light->getLocalToWorldMatrix() * l->localDirection;
            glm::vec3 position = light->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
        
            shader->set(prefix + "type", int(l->light_type));

            shader->set(prefix + "fade.constant", l->constantAttenuation);
            shader->set(prefix + "fade.linear", l->linearAttenuation);
            shader->set(prefix + "fade.quadratic", l->quadraticAttenuation);

            shader->set(prefix + "ambient", l->ambient);
            shader->set(prefix + "diffuse", l->diffuse);
            shader->set(prefix + "specular", l->specular);

            shader->set(prefix + "position", position);
            shader->set(prefix + "direction",  direction);

            shader->set(prefix + "cutOff", glm::cos(glm::radians(l->innerCutoff)));
            shader->set(prefix + "outerCutOff", glm::cos(glm::radians(l->outerCutoff)));
        }


    public:
        void update(World* world, float deltaTime)
        {
            auto& lights = world->getEntitiesByTag(LightComponent::getID());
            auto& camera = world->getEntitiesByTag(CameraComponent::getID());
            auto& meshs  = world->getEntitiesByTag(MeshRendererComponent::getID());

            unordered_set<ShaderProgram*> uniqueShaders;

            for(auto& mesh : meshs)
            {
                ShaderProgram* shader = mesh->getComponent<MeshRendererComponent>()->material->shader;
                uniqueShaders.insert(shader);
            }

            for(auto& shader: uniqueShaders)
            {
                shader->set("n_lights", int(lights.size()));
                for(int i = 0; i < lights.size(); i++)
                {
                    setLightingsUniforms(shader, lights[i], i);
                }
            }
        }
    };
}