#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our {

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const {
        pipelineState.setup();
        shader->use();
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;

        if(data.contains("pipelineState")){
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint 
    void TintedMaterial::setup() const {
        Material::setup();
        shader->set("tint", tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex" 
    void TexturedMaterial::setup() const {
        TintedMaterial::setup();
        
        glActiveTexture(GL_TEXTURE0);
        texture->bind();
        sampler->bind(0);
        
        shader->set("alphaThreshold", alphaThreshold);
        shader->set("tex", 0);
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json& data){
        TintedMaterial::deserialize(data);
        if(!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }


    void LitMaterial::setup() const 
    {    
        Material::setup();

        glActiveTexture(GL_TEXTURE0);
        diffuseMap->bind();
        sampler->bind(0);

        glActiveTexture(GL_TEXTURE1);
        specularMap->bind();
        sampler->bind(1);

        glActiveTexture(GL_TEXTURE2);
        emissionMap->bind();
        sampler->bind(2);

        shader->set("material.diffuse", 0);
        shader->set("material.specular", 1);
        shader->set("material.emission", 2);
        shader->set("material.shininess", shininess);
    }

    void LitMaterial::deserialize(const nlohmann::json& data)
    {
        Material::deserialize(data);
        if(!data.is_object()) return;
        diffuseMap = AssetLoader<Texture2D>::get(data.value("diffuseMap", ""));
        specularMap = AssetLoader<Texture2D>::get(data.value("specularMap", ""));
        emissionMap = AssetLoader<Texture2D>::get(data.value("emissionMap", ""));  
        shininess = data.value("shininess", 0.0f);
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

}
