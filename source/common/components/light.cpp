#include "light.hpp"
#include "../deserialize-utils.hpp"



namespace our {
    // Reads camera parameters from the given json object
    void LightComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        std::string type = data.value("lightType", "directional");
        if(type == "directional"){
            lightType = LightType::DIRECTIONAL;
        } else if(type == "point"){
            lightType = LightType::POINT;
        }
        else {
            lightType = LightType::SPOT;
        }
        color = data.value("color", glm::vec3(1.0f,1.0f,1.0f));
        attenuation = data.value("attenuation", glm::vec3(1, 0, 0));
        cone_angles = data.value("coneAngles", glm::vec2(15.0f, 30.0f));
    }
}