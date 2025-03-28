#include "mesh-renderer.hpp"
#include "../asset-loader.hpp"

namespace our {
    // Receives the mesh & material from the AssetLoader by the names given in the json object
    void MeshRendererComponent::deserialize(const nlohmann::json& data){
        // Notice how we just get a string from the json file and pass it to the AssetLoader to get us the actual asset
        //TODO: (Req 8) Get the material and the mesh from the AssetLoader by their names
        // which are defined with the keys "mesh" and "material" in data.
        // Hint: To get a value of type T from a json object "data" where the key corresponding to the value is "key",
        // you can use write: data["key"].get<T>().
        // Look at "source/common/asset-loader.hpp" to know how to use the static class AssetLoader.
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