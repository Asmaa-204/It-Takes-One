#include <components/mesh-renderer.hpp>
#include <asset-loader.hpp>

namespace our {
    // Receives the mesh & material from the AssetLoader by the names given in the json object
    void MeshRendererComponent::deserialize(const nlohmann::json& data){

        if(!data.is_object()) return;

        if(data.contains("mesh")) {
            std::string meshName = data["mesh"].get<std::string>();
            mesh = AssetLoader<Mesh>::get(meshName);
        }

        if(data.contains("material")) {
            std::string materialName = data["material"].get<std::string>();
            material = AssetLoader<Material>::get(materialName);
        }
    }
}