#include <components/light.hpp>
#include <deserialize-utils.hpp>

namespace our {
    void LightComponent::deserialize(const nlohmann::json& data)
    {
        if (!data.is_object()) return;

        ambient = data.value("ambient", ambient);
        diffuse = data.value("diffuse", diffuse);
        specular = data.value("specular", specular);

        innerCutoff = data.value("innerCutoff", innerCutoff);
        outerCutoff = data.value("outerCutoff", outerCutoff);

        constantAttenuation = data.value("constant", constantAttenuation);
        linearAttenuation = data.value("linear", linearAttenuation);
        quadraticAttenuation = data.value("quadratic", quadraticAttenuation);

        light_type = data.value("light_type", light_type);
        shininess = data.value("shininess", shininess);        
    }    
}