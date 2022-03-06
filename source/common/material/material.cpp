#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our {

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const {
        //TODO: Write this function
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
        //TODO: Write this function
        Material::setup();
        shader->set("tint",this->tint);
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
        //TODO: Write this function
        TintedMaterial::setup();
        shader->set("alpha_threshold",this->alphaThreshold);
        
        glActiveTexture(GL_TEXTURE0);
 
        texture->bind();

        // Then we bind the sampler to unit 0
        sampler->bind(0);
        // Then we send 0 (the index of the texture unit we used above) to the "tex" uniform
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

    void LitMaterial::setup() const {
        //TODO: Write this function
        Material::setup();
        
        glActiveTexture(GL_TEXTURE0);
        albedo_map->bind();

        glActiveTexture(GL_TEXTURE1);
        specular_map->bind();

        glActiveTexture(GL_TEXTURE2);
        ambient_occlusion_map->bind();

        glActiveTexture(GL_TEXTURE3);
        roughness_map->bind();

        glActiveTexture(GL_TEXTURE4);
        emissive_map->bind();

        // Then we bind the sampler to unit 0
        for(GLuint unit = 0; unit < 5; ++unit) sampler->bind(unit);

        // Then we send 0 (the index of the texture unit we used above) to the "tex" uniform
        shader->set("material.albedo_map", 0);
        shader->set("material.specular_map", 1);
        shader->set("material.ambient_occlusion_map", 2);
        shader->set("material.roughness_map", 3);
        shader->set("material.emissive_map", 4);
    }

    // This function read the material data from a json object
    void LitMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        albedo_map = AssetLoader<Texture2D>::get(data.value("albedo_map", ""));
        specular_map = AssetLoader<Texture2D>::get(data.value("specular_map", ""));
        ambient_occlusion_map = AssetLoader<Texture2D>::get(data.value("ambient_occlusion_map", ""));
        roughness_map = AssetLoader<Texture2D>::get(data.value("roughness_map", ""));
        emissive_map = AssetLoader<Texture2D>::get(data.value("emissive_map", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

}